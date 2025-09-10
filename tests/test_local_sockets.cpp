#include <gtest/gtest.h>
#include <array>
#include <string>
#include "../back-end/local-sockets/local_sockets.h"

// Test socket pair initialization
TEST(LocalSocketsTest, InitializeSocketPair) {
    auto sockets = initSocketPair();
    EXPECT_GE(sockets[0], 0);
    EXPECT_GE(sockets[1], 0);

    close(sockets[0]);
    close(sockets[1]);
}

// Test Parent -> Child communication
TEST(LocalSocketsTest, ParentToChild) {
    auto sockets = initSocketPair();
    std::string msg = "Message from Parent";

    Response res = lsCommunicateAtoB(sockets, msg);

    EXPECT_EQ(res.msg, msg);

    // extract number after ':'
    size_t colonPos = res.extra.find(':');
    ASSERT_NE(colonPos, std::string::npos) << "res.extra format unexpected: " << res.extra;

    int bytes = std::stoi(res.extra.substr(colonPos + 1));
    EXPECT_GE(bytes, msg.size());

    close(sockets[0]);
    close(sockets[1]);
}

TEST(LocalSocketsTest, ChildToParent) {
    auto sockets = initSocketPair();
    std::string msg = "Message from Child";

    Response res = lsCommunicateBtoA(sockets, msg);

    EXPECT_EQ(res.msg, msg);

    size_t colonPos = res.extra.find(':');
    ASSERT_NE(colonPos, std::string::npos) << "res.extra format unexpected: " << res.extra;

    int bytes = std::stoi(res.extra.substr(colonPos + 1));
    EXPECT_EQ(bytes, msg.size());

    close(sockets[0]);
    close(sockets[1]);
}
