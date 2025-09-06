#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <shared_memory.h>
#include <main.h>

using json = nlohmann::json;

char* fifoPath = "/tmp/fifo";


inline Request deserializeRequest(const std::string& s) {
    auto j = json::parse(s);
    return { j["id"], j["message"] };
}

std::string getRequest(char* path) {
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


int main(int argc, char const *argv[])
{
    auto backend = []() {
        std::cout << "Started backend" << std::endl;
        mkfifo(fifoPath, 0666);
        std::cout << "Started FIFO communication with frontend" << std::endl;

        while (true) {
            std::string r = getRequest(fifoPath);
            if (r == "err") {
                std::cout << "Error reading request, closing communication";
                exit(1);
            }
            Request req = deserializeRequest(r);
            if (!req.requestReady) {
                continue;
            }

            Endpoint endpoint = req.body.endpoint;
            switch (endpoint)
            {
            case sharedMemory:
                
                break;
            
            case anonymousPipes:
                /* code */
                break;
            
            case localSockets:
                /* code */
                break;
            
            default:
                break;
            }

        }
    };
    backend();
    return 0;
}
