#include <gtest/gtest.h>
#include <array>
#include <string>
#include "../back-end/local-sockets/local_sockets.h"

// Test socket pair initialization
TEST(LocalSocketsTest, InitializeSocketPair) {
    auto sockets = initializeSocketPair();
    EXPECT_GE(sockets[0], 0);
    EXPECT_GE(sockets[1], 0);

    close(sockets[0]);
    close(sockets[1]);
}

// Test Parent -> Child communication
TEST(LocalSocketsTest, ParentToChild) {
    auto sockets = initializeSocketPair();
    std::string msg = "Message from Parent";

    Response res = communicateAtoB(sockets, msg);

    EXPECT_EQ(res.msg, msg);
    EXPECT_GE(std::stoi(res.extra), msg.size()); // bytes written >= message size
}

// Test Child -> Parent communication
TEST(LocalSocketsTest, ChildToParent) {
    auto sockets = initializeSocketPair();
    std::string msg = "Message from Child";

    Response res = communicateBtoA(sockets, msg);

    EXPECT_EQ(res.msg, msg);

    // extract numeric value from "bytes=16"
    size_t eqPos = res.extra.find('=');
    ASSERT_NE(eqPos, std::string::npos);
    int bytes = std::stoi(res.extra.substr(eqPos + 1));

    EXPECT_EQ(bytes, msg.size()); // only message length, no null terminator
}
