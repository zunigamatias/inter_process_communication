# inter_process_communication


The communication between backend and frontend will be made through shared memory. Here is what the shared object will look like:
```c++
Struct Request {
    uint id;
    Body body;
    bool requestReady;
    bool responseReady;
}
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