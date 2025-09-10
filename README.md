# inter_process_communication

The project demonstrates **inter-process communication (IPC)** between a **C++ backend** and a **Node.js bridge/frontend**.  
The communication between backend and frontend is done through **FIFOs**, and the backend supports multiple IPC mechanisms.  

---

# ⚙️ Build & Run Instructions
## To run the program, type this om the root of the repository: 
```
./run.sh
```

## 🧪 Tests

Unit tests use GoogleTest (gtest).

Run all tests with:
```
./run_tests.sh
```

---
# 📦 Request/Response Structures

## Request Structures

### `RequestBody`
```c++
struct RequestBody {
    std::string mainProcess;
    Endpoint endpoint;
    std::string message;
    std::string endpointString;
};
```
### Field explanations:

- mainProcess → Indicates which process is sending the message. For example, "A" or "B".

- endpoint → Enum indicating the communication method to use:

- sharedMemory → Use shared memory for IPC

- anonymousPipes → Use anonymous pipes for IPC

- localSockets → Use local (UNIX) sockets for IPC

- message → The actual content of the message sent by the process.

- endpointString → Optional string representation of the endpoint, useful for logging or JSON serialization.

### `Request`
```c++
struct Request {
    RequestBody body;
    bool requestReady;
    bool responseReady;
    bool endCommunication;
};

```

### Field explanations:

- body → The RequestBody struct containing the main message and metadata.

- requestReady → Flag indicating that a request has been sent from the frontend and is ready to be processed by the backend.

- responseReady → Flag indicating that the backend has processed the request and the response is ready to be read.

- endCommunication → Flag to indicate that the communication session should be terminated (e.g., shutting down the backend or ending IPC).

## Response Structure
### `Response`
```c++
struct Response {
    std::string msg;
    std::string extra;
    std::string communicationMethod;
    std::string sender;
};
```
### Field explanations:

- msg → The main message returned by the backend, usually echoing or processing the original request.

- extra → Any additional information or metadata, such as debug info, status messages, or processing results.

- communicationMethod → A string indicating which IPC method was actually used to send the response (e.g., "sharedMemory", "localSockets", "anonymousPipes").

- sender → The process that generated the response (e.g., "A" or "B")