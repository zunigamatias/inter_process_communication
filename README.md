# inter_process_communication


The communication between backend and frontend will be made through shared memory. Here is what the shared object will look like:
```c++
struct Request {
    uint id;
    Body body;
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
Body: {
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

## Local sockets: 
```
g++ -pthread back-end/local-sockets/local_sockets.cpp tests/test_local_sockets.cpp -lgtest -lgtest_main -o tests/test_local_sockets.out
./tests/test_local_sockets.out
```

## Anonoymous pipes:
```
g++ -pthread back-end/anonymous-pipes/anonymous_pipes.cpp tests/test_anonymous_pipes.cpp -lgtest -lgtest_main -o tests/test_anonymous_pipes.out
./tests/test_anonymous_pipes.out
```

## Integration:
```
g++ -pthread \
    back-end/main.cpp \
    back-end/shared-memory/shared_memory.cpp \
    back-end/anonymous-pipes/anonymous_pipes.cpp \
    back-end/local-sockets/local_sockets.cpp \
    tests/test_backend_main.cpp \
    -lgtest -lgtest_main -o tests/test_mainfile.out
./tests/test_mainfile.out
```
