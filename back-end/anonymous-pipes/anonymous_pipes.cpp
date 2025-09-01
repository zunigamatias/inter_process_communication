#include "anonymous_pipes.h"

#include <fcntl.h>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>


void writeMessage(int processPipe[2], const char* msg) {
    close(processPipe[0]);
    write(processPipe[1], msg, strlen(msg) + 1);
    close(processPipe[1]);
}

std::string readMessage(int processPipe[2]) {
    char buffer[100];
    close(processPipe[1]);
    read(processPipe[0], buffer, sizeof(buffer));
    close(processPipe[0]);

    return std::string(buffer);
}

int main(int argc, char const *argv[])
{
    const char *msg = "Message sent";
    int pipeAB[2];
    int pipeBA[2];
    pipe(pipeAB);
    pipe(pipeBA);

    pid_t child1 = fork();
    if (child1 == 0) {
        writeMessage(pipeAB, msg);
        exit(0);
    }
    
    pid_t child2 = fork();
    if (child2 == 0) {
        std::string some = readMessage(pipeAB);
        std::cout << some << std::endl;
        exit(0);
    }

    return 0;
}
