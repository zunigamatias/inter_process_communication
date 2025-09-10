#include <gtest/gtest.h>
#include <array>
#include <string>
#include "../back-end/anonymous-pipes/anonymous_pipes.h"

// Test writeMessage and readMessage directly
TEST(AnonymousPipesTest, CommunicateAtoB) {
    auto pipeAB = initAnonymousPipes();
    auto pipeBA = initAnonymousPipes();

    std::string msg = "from A to B";
    Response res = apCommunicateAtoB(pipeAB, pipeBA, msg);

    EXPECT_EQ(res.msg, msg);

    size_t colonPos = res.extra.find(':');
    ASSERT_NE(colonPos, std::string::npos) << "Unexpected format: " << res.extra;
    int bytes = std::stoi(res.extra.substr(colonPos + 1));
    EXPECT_EQ(bytes, msg.size() + 1); // includes '\0'
}

TEST(AnonymousPipesTest, CommunicateBtoA) {
    auto pipeAB = initAnonymousPipes();
    auto pipeBA = initAnonymousPipes();

    std::string msg = "from B to A";
    Response res = apCommunicateBtoA(pipeAB, pipeBA, msg);

    EXPECT_EQ(res.msg, msg);

    size_t colonPos = res.extra.find(':');
    ASSERT_NE(colonPos, std::string::npos) << "Unexpected format: " << res.extra;
    int bytes = std::stoi(res.extra.substr(colonPos + 1));
    EXPECT_EQ(bytes, msg.size() + 1); // includes '\0'
}
