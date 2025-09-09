#ifndef REQUEST_H
#define REQUEST_H

#include <string>


enum Endpoint {
    sharedMemory,
    anonymousPipes,
    localSockets
};

struct RequestBody {
    std::string mainProcess;
    Endpoint endpoint;
    std::string message;
};

// here i'm setting requestready and responseReady: 
// when a request is sent from the front-end, 
// requestReady will be True.
// when the response is sent from the back-end,
// responseReady will be True.
struct Request {
    RequestBody body;
    bool requestReady;
    bool responseReady;
    bool endCommunication;
};

#endif