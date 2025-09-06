#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Body{
    std::string mainProcess;
    std::string endpoint;
    std::string message;
};

struct Request{
    uint id;
    std::string body;
    bool requestReady;
    bool responseReady;
};


int main(int argc, char const *argv[])
{
    json receivedRequest; 
    
    return 0;
}
