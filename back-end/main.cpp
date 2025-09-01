#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <cstring>


struct SharedData {
    pthread_mutex_t lock; 
    bool ready;           
    char message[256];    
};


int main() {
    int pipeAB[2];
    pipe(pipeAB);  // create before forking

    pid_t childA = fork();
    if (childA == 0) {
        // Child A (writer)
        close(pipeAB[0]);  // don’t read
        const char *msg = "Hello from A!";
        write(pipeAB[1], msg, strlen(msg) + 1);
        close(pipeAB[1]);
        _exit(0);
    }

    pid_t childB = fork();
    if (childB == 0) {
        // Child B (reader)
        close(pipeAB[1]);  // don’t write
        char buf[100];
        read(pipeAB[0], buf, sizeof(buf));
        std::cout << "Child B got: " << buf << std::endl;
        close(pipeAB[0]);
        _exit(0);
    }

    // Parent closes both ends, waits
    close(pipeAB[0]);
    close(pipeAB[1]);
    waitpid(childA, NULL, 0);
    waitpid(childB, NULL, 0);
}
