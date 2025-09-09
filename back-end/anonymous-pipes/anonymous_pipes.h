#ifndef ANONYMOUS_PIPES_H
#define ANONYMOUS_PIPES_H

#include <string>
#include "../../integration/response.h"

ssize_t writeMessage(int fd, const char* msg);

std::pair<std::string, std::string> readMessage(int fd);

std::array<int, 2> initAnonymousPipes();

Response apCommunicateAtoB(std::array<int, 2> pipeAB, std::array<int, 2> pipeBA, std::string msg);

Response apCommunicateBtoA(std::array<int, 2> pipeAB, std::array<int, 2> pipeBA, std::string msg);

#endif 