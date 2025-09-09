#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <array>
#include "local_sockets.h"

// 1. Initialize local socket pair
std::array<int, 2> initSocketPair() {
    std::array<int, 2> sockets;
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets.data()) == -1) {
        perror("socketpair");
    }
    return sockets;
}

// 2. Communication Parent -> Child
Response lsCommunicateAtoB(std::array<int, 2> sockets, std::string msg) {
    pid_t pid = fork();
    if (pid == 0) {
        // --- Child ---
        close(sockets[0]); // child uses sockets[1]

        char buffer[100];
        ssize_t bytesRead = read(sockets[1], buffer, sizeof(buffer));
        if (bytesRead > 0) {
            // echo back to parent
            write(sockets[1], buffer, bytesRead);
        }

        close(sockets[1]);
        _exit(0);
    } else {
        // --- Parent ---
        close(sockets[1]); // parent uses sockets[0]

        ssize_t bytesWritten = write(sockets[0], msg.c_str(), msg.size() + 1);

        char buffer[100];
        ssize_t bytesRead = read(sockets[0], buffer, sizeof(buffer));

        wait(NULL);
        close(sockets[0]);

        return Response{
            std::string(buffer), 
            "Bytes written by A: " + std::to_string(bytesWritten)
        };
    }
}

// 3. Communication Child -> Parent
Response lsCommunicateBtoA(std::array<int, 2> sockets, const std::string& msg) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return {"", "fork failed"};
    }

    if (pid == 0) {
        // ----- Child -----
        close(sockets[0]); // child uses sockets[1]
        ssize_t n = write(sockets[1], msg.c_str(), msg.size() + 1); // include '\0'
        if (n == -1) perror("write");
        // Tell the parent we're done writing
        shutdown(sockets[1], SHUT_WR);
        close(sockets[1]);
        _exit(0);
    } else {
        // ----- Parent -----
        close(sockets[1]); // parent uses sockets[0]

        // Read until EOF or until we see the '\0' terminator
        std::string out;
        char buf[128];
        while (true) {
            ssize_t n = read(sockets[0], buf, sizeof buf);
            if (n == -1) {
                if (errno == EINTR) continue;
                perror("read");
                break;
            }
            if (n == 0) break; // EOF (child shutdown/closed)
            out.append(buf, buf + n);
            if (!out.empty() && out.back() == '\0') {
                out.pop_back();          // drop the '\0'
                break;
            }
        }

        waitpid(pid, nullptr, 0);
        close(sockets[0]);
        return Response{
            out, 
            "Bytes written by B: " + std::to_string(out.size())
        };
    }
}
