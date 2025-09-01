#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>


struct SharedData {
    pthread_mutex_t lock;
    pthread_cond_t cond;   // Condition variable for signaling
    bool requestReady;
    bool responseReady;
    char request[256];
    char response[256];
};


int main() {
    
}
