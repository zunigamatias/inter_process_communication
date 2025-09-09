#include <array>
#include <fcntl.h>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>

#include "anonymous_pipes.h"

// writes message into the file descriptor
ssize_t writeMessage(int fd, const char* msg) {
    return write(fd, msg, strlen(msg) + 1); // write including '\0'
}

// reads message and returns the quantity of bytes read
std::pair<std::string, std::string> readMessage(int fd) {
    char buffer[100];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
    if (bytesRead > 0) {
        return {std::string(buffer), std::to_string(bytesRead)};
    } else {
        return {"", std::to_string(bytesRead)};
    }
}

// creates and returns the pipes
std::array<int, 2> initializeAnonymousPipes() {
    std::array<int, 2> pipeFD;
    if (pipe(pipeFD.data()) == -1) {
        perror("pipe");
    }
    return pipeFD;
}

// sets the communication between parent to child
Response communicateAtoB(std::array<int, 2> pipeAB, std::array<int, 2> pipeBA, std::string msg) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child: B reads from pipeAB, writes to pipeBA
        close(pipeAB[1]); // close write end of AB
        close(pipeBA[0]); // close read end of BA

        auto [tmpMsg, _] = readMessage(pipeAB[0]);
        writeMessage(pipeBA[1], tmpMsg.c_str());

        close(pipeAB[0]);
        close(pipeBA[1]);
        _exit(0);
    } else {
        // Parent: A writes to pipeAB, reads from pipeBA
        close(pipeAB[0]); // close read end of AB
        close(pipeBA[1]); // close write end of BA

        ssize_t bytesWritten = writeMessage(pipeAB[1], msg.c_str());
        close(pipeAB[1]);

        auto [finalMsg, _] = readMessage(pipeBA[0]);
        close(pipeBA[0]);

        wait(NULL);

        return Response {
            finalMsg,
            std::to_string(bytesWritten)
        };
    }
}


Response communicateBtoA(std::array<int, 2> pipeAB, std::array<int, 2> pipeBA, std::string msg) {
    pid_t pid = fork();
    // --- Child process ---
    if (pid == 0) {
        close(pipeBA[0]); // close read end of BA
        close(pipeAB[1]); // close write end of AB

        ssize_t bytesWritten = writeMessage(pipeBA[1], msg.c_str());
        close(pipeBA[1]);

        auto [tmpMsg, _] = readMessage(pipeAB[0]);
        close(pipeAB[0]);

        _exit(0);
    } else {
        // --- Parent process ---
        close(pipeBA[1]); // close write end of BA
        close(pipeAB[0]); // close read end of AB

        // A writes a reply to B
        ssize_t bytesWritten = writeMessage(pipeAB[1], msg.c_str());
        close(pipeAB[1]);

        auto [finalMsg, _] = readMessage(pipeBA[0]);
        close(pipeBA[0]);

        wait(NULL);

        return Response {
            finalMsg,
            std::to_string(bytesWritten)
        };
    }
}
