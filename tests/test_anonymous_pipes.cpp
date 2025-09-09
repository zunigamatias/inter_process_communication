#include <gtest/gtest.h>
#include <array>
#include <string>
#include "../back-end/anonymous-pipes/anonymous_pipes.h"

// Test writeMessage and readMessage directly
TEST(AnonymousPipesTest, WriteAndReadMessage) {
    auto pipeFD = initAnonymousPipes();

    const char* msg = "hello pipes";

    // Write in parent
    ssize_t bytesWritten = writeMessage(pipeFD[1], msg);
    EXPECT_GT(bytesWritten, 0);

    // Read in parent (same process, not fork)
    auto [readMsg, bytesRead] = readMessage(pipeFD[0]);
    EXPECT_EQ(readMsg, "hello pipes");
    EXPECT_EQ(std::stoi(bytesRead), bytesWritten);

    close(pipeFD[0]);
    close(pipeFD[1]);
}

// Test that pipes are initialized correctly
TEST(AnonymousPipesTest, InitializePipes) {
    auto pipeFD = initAnonymousPipes();
    EXPECT_GE(pipeFD[0], 0);
    EXPECT_GE(pipeFD[1], 0);

    close(pipeFD[0]);
    close(pipeFD[1]);
}

// Test A->B communication
TEST(AnonymousPipesTest, CommunicateAtoB) {
    auto pipeAB = initAnonymousPipes();
    auto pipeBA = initAnonymousPipes();

    std::string msg = "from A to B";
    Response res = apCommunicateAtoB(pipeAB, pipeBA, msg);

    EXPECT_EQ(res.msg, msg);
    EXPECT_EQ(std::stoi(res.extra), msg.size() + 1); // includes '\0'
}

// Test B->A communication
TEST(AnonymousPipesTest, CommunicateBtoA) {
    auto pipeAB = initAnonymousPipes();
    auto pipeBA = initAnonymousPipes();

    std::string msg = "from B to A";
    Response res = apCommunicateBtoA(pipeAB, pipeBA, msg);

    EXPECT_EQ(res.msg, msg);
    EXPECT_EQ(std::stoi(res.extra), msg.size() + 1); // includes '\0'
}
