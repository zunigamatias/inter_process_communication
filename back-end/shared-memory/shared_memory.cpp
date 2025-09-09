#include <cstring>    // For strcpy
#include <fcntl.h>    // For O_* constants
#include <iostream>
#include <pthread.h>  // For pthread_mutex
#include <string>
#include <sys/mman.h> // For shm_open, mmap
#include <sys/stat.h> // For mode constants
#include <sys/wait.h>
#include <unistd.h>   // For ftruncate, fork, sleep

#include "shared_memory.h"

const char* memory_path = "/data";


SharedData* initSharedMemory() {
    const size_t SIZE = sizeof(SharedData);
    bool isCreator = false;
    
    //creates a new or open a new shared memory object
    int shmFileDescriptor = shm_open(memory_path, O_RDWR | O_CREAT | O_EXCL, 0666);
    if (shmFileDescriptor >= 0) {
        isCreator = true;
    } else if (errno == EEXIST) {
        shmFileDescriptor = shm_open(memory_path, O_RDWR, 0666);
        if (shmFileDescriptor == -1) {
            perror("shm_open");
            return nullptr;
        }
    } else {
        perror("shm_open");
        return nullptr;
    }

    // the size of shared memory created by shm_open is zero. 
    // ftruncate sets the size of the shared memory
    if (ftruncate(shmFileDescriptor, SIZE) == -1) {
            perror("ftruncate");
            return nullptr;
        }
    // maps a kernel address space to a user address space.
    // The shared memory is created in kernel virtual memory 
    // and each process maps the same memory location to its own memory space
    // this eliminates the overhead of copying information
    void* ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmFileDescriptor, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return nullptr;
    }
    // the pointer does not know what is going to be at the memory address
    // so it returns a void * ptr, indicating the beggining of the memory address 
    // of that object
    SharedData* data = static_cast<SharedData*>(ptr);
    
    if (isCreator){
        // creates a mutex structure
        pthread_mutexattr_t attr;
        // initialize the mutex with default attributes
        pthread_mutexattr_init(&attr);
        // sets the mutex to be shared between processes, 
        // allow the mutex to be placed in the shared memory
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        // Initializes the mutex
        pthread_mutex_init(&data->lock, &attr);
        pthread_mutexattr_destroy(&attr);

        // Optional: zero out msg buffer
        memset(data->msg, 0, sizeof(data->msg));
    }
    return data;
}

void sendData(SharedData* data, const std::string& msg) {
    //locks the mutex to prevent multiple processes
    // from accessing a shared resource at the same time.
    pthread_mutex_lock(&data->lock);
    // access the shared data
    strncpy(data->msg, msg.c_str(), sizeof(data->msg) - 1);
    data->msg[sizeof(data->msg) - 1] = '\0';
    //unlocks the mutex for future use
    pthread_mutex_unlock(&data->lock);
}

std::string readData(SharedData* data) {
    //locks the mutex to prevent multiple processes
    // from accessing a shared resource at the same time.
    pthread_mutex_lock(&data->lock);
    // access the shared data
    std::string msg = data->msg;
    //unlocks the mutex for future use
    pthread_mutex_unlock(&data->lock);
    return msg;
}

std::string getSharedMemoryStatus(SharedData* data) {
    std::string statusMsg;

    //returns a string depending of the lock state of the mutex
    int lockState = pthread_mutex_trylock(&data->lock);
    if (lockState == 0) {
        statusMsg = "Mutex state: Unlocked";
        pthread_mutex_unlock(&data->lock);
    } else if (lockState == EBUSY) {
        statusMsg = "Mutex state: Locked by another process/thread";
    } else {
        statusMsg = "Mutex state: Unknown, error: "+ std::string(strerror(lockState));
    }

    return statusMsg;
}

Response communicateAtoB(std::string msg) {
    SharedData* data = initSharedMemory();
    if (!data) return Response{};

    sendData(data, msg);

    //creates a child process
    pid_t pid = fork();
    if (pid == 0) {
        std::string res = readData(data);
        sendData(data, res);
        _exit(0);
    }
    // waits for the child process to return the read message
    wait(NULL);
    std::string sentData = readData(data);
    std::string status = getSharedMemoryStatus(data);
    return Response{
        sentData,
        status
    };
}

Response communicateBtoA(std::string msg) {
    std::string sentData;
    SharedData* data = initSharedMemory();
    if (!data) return Response{}; 

    //creates a child process
    pid_t pid = fork();
    if (pid == 0) {
        sendData(data,msg);
        _exit(0);
    }
    if (pid > 0) {
        // waits for the child process to send the data
        wait(NULL);
        sentData = readData(data);
        std::string status = getSharedMemoryStatus(data);
        return Response{
            sentData,
            status
        };
    }
    return Response{};
}
