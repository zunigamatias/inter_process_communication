#ifndef LOCAL_SOCKETS_H
#define LOCAL_SOCKETS_H

#include "../../integration/response.h"

std::array<int, 2> initSocketPair();

Response lsCommunicateAtoB(std::array<int, 2> sockets, std::string msg);

Response lsCommunicateBtoA(std::array<int, 2> sockets, const std::string& msg);



#endif