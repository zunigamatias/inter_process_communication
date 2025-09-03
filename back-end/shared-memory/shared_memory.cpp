#include <cstring>    // For strcpy
#include <fcntl.h>    // For O_* constants
#include <fstream>
#include <iostream>
#include <map>
#include <pthread.h>  // For pthread_mutex
#include <string>
#include <sys/mman.h> // For shm_open, mmap
#include <sys/stat.h> // For mode constants
#include <unistd.h>   // For ftruncate, fork, sleep

const char* memory_path = "/data";

// here i'm setting requestready and responseReady: 
// when a request is sent from the front-end, 
// requestReady will be True.
// when the response is sent from the back-end,
// responseReady will be True.
struct SharedData {
    pthread_mutex_t lock;
    pthread_cond_t cond;   // Condition variable for signaling
    bool requestReady;
    bool responseReady;
    char request[256];
    char response[256];
};
SharedData* initSharedMemory() {
    const size_t SIZE = sizeof(SharedData);
    
    //creates a new or open a new shared memory object
    int shmFileDescriptor = shm_open(memory_path, O_CREAT | O_RDWR, 0666);
    if (shmFileDescriptor == -1) {
            perror("shm_open");
            return nullptr;
        }
        
    // the size of shared memory created by shm_open is zero. 
    // ftruncate sets the size of the shared memory
    if (ftruncate(shmFileDescriptor, SIZE) == -1) {
            perror("ftruncate");
            return nullptr;
        }
        
    // maps a kernel address space to a user address space
    // the shared memory is created in kernel virtual memory 
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
    
    // creates a mutex structure
    pthread_mutexattr_t attr;
    // initialize the mutex with default attributes
    pthread_mutexattr_init(&attr);
    // sets the mutex to be shared between processes,
    // allow the mutex to be placed in shared memory
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    // Initializes the mutex
    pthread_mutex_init(&data->lock, &attr);


    // Initial state
    data->requestReady = true;
    data->responseReady = false;

    return data;
}

void processRequest(SharedData* data, const std::string& responseText) {
    pthread_mutex_lock(&data->lock);

    if (data->requestReady) {
        // Write response
        strncpy(data->response, responseText.c_str(), sizeof(data->response));
        std::cout << "Backend received request: " << data->response << std::endl;
        data->response[sizeof(data->response) - 1] = '\0';
        data->requestReady = false;
        data->responseReady = true;
    }

    pthread_mutex_unlock(&data->lock);
}


int main() {
    
    auto backend = []() {
        SharedData* data = initSharedMemory();
        if (!data) return;

        while (true) {
            processRequest(data, "Hello from backend!");
            sleep(1); // simulate processing
        }
    };

    backend(); // run the backend loop
}