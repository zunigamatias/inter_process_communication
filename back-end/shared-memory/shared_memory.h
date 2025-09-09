#ifndef shared_memory
#define shared_memory

#include <pthread.h>
#include <string>

#include "../../integration/response.h"

// struct that contains the msg sent
// and a mutex to safety communication
struct SharedData {
    pthread_mutex_t lock;
    char msg[256];
};

// Sets the data that is being read with the message
// that the user set in the frontend, it locks and unlocks the mutex
// to ensure safety between the shared memory
void sendData(SharedData* data, const std::string& msg);

// Returns the message in the shared struct
std::string readData(SharedData* data);

// Gets the status of the shared memory
std::string getSharedMemoryStatus(SharedData* data);

// Sends message from the parent process to the child process
Response shmCommunicateAtoB(std::string msg);

// Sends message from the child process to the parent process
Response shmCommunicateBtoA(std::string msg);

SharedData* initSharedMemory();

#endif
