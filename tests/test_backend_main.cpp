#include <gtest/gtest.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <nlohmann/json.hpp>

#include "../integration/request.h"
#include "../integration/response.h"
#include "../back-end/main.h"


using json = nlohmann::json;

extern int main(int argc, const char* argv[]); // declare backend main()

// Helper: write JSON into FIFO
void writeFifo(const char* path, const json& j) {
    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror("open FIFO for writing");
    }
    ASSERT_NE(fd, -1) << "Failed to open FIFO for writing: " << path;

    std::string data = j.dump();
    ssize_t bytesWritten = write(fd, data.c_str(), data.size() + 1);
    if (bytesWritten <= 0) {
        perror("write FIFO");
    }
    ASSERT_GT(bytesWritten, 0) << "Failed to write to FIFO: " << path;

    close(fd);
}

// Helper: read JSON from FIFO
json readFifo(const char* path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open FIFO for reading");
        throw std::runtime_error("Failed to open FIFO for reading: " + std::string(path));
    }

    char buffer[512] = {0};
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
    if (bytesRead <= 0) {
        perror("read FIFO");
        close(fd);
        throw std::runtime_error("Failed to read from FIFO: " + std::string(path));
    }

    close(fd);
    return json::parse(std::string(buffer));
}


TEST(BackendIntegrationTest, RequestResponseAndShutdown) {
    // Remove stale FIFOs
    unlink(fifoPathReq);
    unlink(fifoPathRes);

    // Start backend in child process
    pid_t pid = fork();
    ASSERT_GE(pid, 0) << "Fork failed";

    if (pid == 0) {
        const char* argv[] = {"backend", nullptr};
        main(1, argv);  // runs infinite loop until shutdown request
        _exit(0);
    }

    // Parent process = acts as frontend
    sleep(1); // allow backend time to create FIFOs

    // Step 1: Send normal request
    json jReq;
    jReq["id"] = 1;
    jReq["endCommunication"] = false;
    jReq["message"]["endpoint"] = "localSockets";   // Must match enum serialization
    jReq["message"]["mainProcess"] = "A";
    jReq["message"]["message"] = "Hello Backend";

    writeFifo(fifoPathReq, jReq);

    // Step 2: Read backend response
    json jRes = readFifo(fifoPathRes);
    Response res{ jRes["msg"], jRes["extra"] };

    EXPECT_EQ(res.msg, "Hello Backend");
    EXPECT_FALSE(res.extra.empty());

    // Step 3: Send shutdown request
    json jShutdown;
    jShutdown["id"] = 2;
    jShutdown["endCommunication"] = true;
    jShutdown["message"]["endpoint"] = "localSockets";
    jShutdown["message"]["mainProcess"] = "A";
    jShutdown["message"]["message"] = "Shutdown";

    writeFifo(fifoPathReq, jShutdown);

    // Step 4: Wait for backend to exit cleanly
    int status = 0;
    waitpid(pid, &status, 0);
    EXPECT_TRUE(WIFEXITED(status));
    EXPECT_EQ(WEXITSTATUS(status), 0);
}
