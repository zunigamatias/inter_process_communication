#ifndef RESPONSE_H
#define RESPONSE_H

#include <string> 

// struct that contains the return msg, 
// which should be the same as the sent msg
// and a extra, that can contain anything useful
// related to the process status
struct Response {
    std::string msg;
    std::string extra;
};

#endif