#ifndef ANONYMOUS_PIPES_H
#define ANONYMOUS_PIPES_H

#include <string>
#include "../../integration/response.h"

ssize_t writeMessage(int fd, const char* msg);

std::pair<std::string, std::string> readMessage(int fd);

std::array<int, 2> initializeAnonymousPipes();

Response communicateAtoB(std::array<int, 2> pipeAB, std::array<int, 2> pipeBA, std::string msg);

Response communicateBtoA(std::array<int, 2> pipeAB, std::array<int, 2> pipeBA, std::string msg);

#endif 