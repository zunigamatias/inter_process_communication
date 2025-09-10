#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "anonymous-pipes/anonymous_pipes.h"
#include "main.h"
#include "local-sockets/local_sockets.h"
#include "shared-memory/shared_memory.h"
#include "../integration/request.h"
#include "../integration/response.h"

using json = nlohmann::json;

const char* fifoPathReq = "/tmp/fifo_req";
const char* fifoPathRes = "/tmp/fifo_res";

inline std::string serializeResponse(const Response& res) {
    json j;
    j["msg"] = res.msg;
    j["extra"] = res.extra;
    j["communicationMethod"] = res.communicationMethod;
    j["sender"] = res.sender;
    return j.dump();
}

inline Request deserializeRequest(const std::string& s) {
    auto j = json::parse(s);

    RequestBody body;
    body.mainProcess = j["message"]["mainProcess"].get<std::string>();
    std::string endpointStr = j["message"]["endpoint"].get<std::string>();
    if (endpointStr == "shared memory") body.endpoint = sharedMemory;
    else if (endpointStr == "anonymous pipes") body.endpoint = anonymousPipes;
    else if (endpointStr == "local sockets") body.endpoint = localSockets;

    body.message = j["message"]["message"].get<std::string>();

    Request req;
    req.body = body;
    req.requestReady = false;
    req.responseReady = true;
    req.endCommunication = j["endCommunication"].get<bool>();
    req.body.endpointString = endpointStr;

    return req;
}

std::string getRequest(const char* path) {    
    int fd = open(path, O_RDONLY);
    char buffer[256] = {0};
    size_t err = read(fd, buffer, sizeof(buffer));
    if (err == -1) {
        perror("open FIFO");
        close(fd);
        return "err";
    }
    close(fd);
    return std::string(buffer);
}

void sendResponse(const std::string& s, const char* path) {
    // Check if FIFO exists, create if not
    if (access(path, F_OK) == -1) {
        if (mkfifo(path, 0666) == -1) {
            perror("mkfifo failed");
            return;
        }
    }

    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror("open FIFO for write");
        return;
    }

    ssize_t bytesWritten = write(fd, s.c_str(), s.size()); // include null terminator
    if (bytesWritten == -1) {
        perror("write failed");
    }
    std::cout << "Response sent" << std::endl;
    close(fd);
}


int main(int argc, char const *argv[])
{
    auto backend = []() {
        std::cout << "Started backend" << std::endl;
        mkfifo(fifoPathReq, 0666);
        mkfifo(fifoPathRes, 0666);
        std::cout << "Started FIFO communication with frontend" << std::endl;

        while (true) {
            std::string r = getRequest(fifoPathReq);
            if (r == "err") {
                std::cout << "Error reading request, closing communication";
                exit(1);
            }
            Request req = deserializeRequest(r);

            if (req.endCommunication) {
                std::cout << "Finishing communication, closing program" <<std::endl;
                exit(0);
            }

            Endpoint endpoint = req.body.endpoint;
            std::string endpointStr = req.body.endpointString;
            std::string sender = req.body.mainProcess;
            std::string msg = req.body.message;

            
            Response res;
            
            switch (endpoint)
            {
            case sharedMemory: {
                SharedData* sharedData = initSharedMemory();
                if (sender == "A") {
                    res = shmCommunicateAtoB(msg);
                }
                else if (sender == "B") {
                    res = shmCommunicateBtoA(msg);
                }
                else {
                    perror("Unknown sender");
                }
                break;
            }
            case anonymousPipes: {
                std::array<int, 2> pipeAB = initAnonymousPipes();
                std::array<int, 2> pipeBA = initAnonymousPipes();
                if (sender == "A") {
                    res = apCommunicateAtoB(pipeAB, pipeBA, msg);
                }
                else if (sender == "B") {
                    res = apCommunicateBtoA(pipeAB, pipeBA, msg);
                }
                else {
                    perror("Unknown sender");
                }
                break;
            }
            case localSockets: {
                std::array<int, 2> socketPair = initSocketPair();
                if (sender == "A") {
                    res = lsCommunicateAtoB(socketPair, msg);
                }
                else if (sender == "B") {
                    res = lsCommunicateBtoA(socketPair, msg);
                }
                else {
                    perror("Unknown sender");
                }
                break;
            }
            default:
            break;
        }
            //setting the endpoint and sender 
            res.communicationMethod = endpointStr;
            res.sender = sender;

            std::cout << "Response message: " << res.msg << std::endl;
            std::cout << "Response extra: " << res.extra << std::endl;
            std::cout << "Sender: " << res.sender << std::endl;
            std::cout << "Endpoint: " << res.communicationMethod << std::endl;

            std::string resStr = serializeResponse(res);
            sendResponse(resStr, fifoPathRes);
            std::cout << "Response succesfully sent" << std::endl;
        }
    };
    backend();
    return 0;
}
