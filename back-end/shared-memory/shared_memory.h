#ifndef shared_memory
#define shared_memory

#include <pthread.h>

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

SharedData* initSharedMemory();

#endif