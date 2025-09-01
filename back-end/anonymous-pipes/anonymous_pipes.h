#ifndef declaration
#define declaration

#include <string>


void writeMessage(int processPipe[2], const char* msg);
std::string readMessage(int processPipe[2]);

#endif 