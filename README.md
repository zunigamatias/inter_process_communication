# inter_process_communication


The communication between backend and frontend will be made through shared memory. Here is what the shared object will look like:
```c++
struct Request {
    uint id;
    std::string body;
    bool requestReady;
    bool responseReady;
}
```
The response from the backend will look like this: 
```c++
struct Response {
    std::string msg;
    std::string extra;
};
```


## Explanation of each field: 
- #### id: represents the request id
- #### body: a json like string that is parsed at read time, the json will have the following fields: 
```markdown
body: {
    std::string mainProcess;
    endpoint: <sharedMemory, localSockets, anonymousPipes>
    message: <message sent>
}
```
Explanation of each field: 
- #### mainProcess: says what is the process that is going to send the message
- #### endpoint: Communication method between processes
- #### message: Message set by the user

# Tests
Running tests for the different communication methods

## Shared memory:
```
g++ -pthread back-end/shared-memory/shared_memory.cpp tests/test_shared_memory.cpp -lgtest -lgtest_main -o tests/test_shared_memory.out
./tests/test_shared_memory.out
```
