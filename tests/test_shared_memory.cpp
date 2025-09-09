#include "../back-end/shared-memory/shared_memory.h"
#include <gtest/gtest.h>
#include <sys/mman.h> 
#include <unistd.h>

// Helper: initialize shared memory fresh for each test
class SharedMemoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Ensure fresh shared memory before each test
        shm_unlink("/data");
    }

    void TearDown() override {
        // Cleanup shared memory after test
        shm_unlink("/data");
    }
};

// --- Tests ---

TEST_F(SharedMemoryTest, InitSharedMemory_CreatesAndInitializes) {
    SharedData* data = initSharedMemory();
    ASSERT_NE(data, nullptr);
    EXPECT_STREQ(data->msg, ""); // should be empty initially
}

TEST_F(SharedMemoryTest, SendData_WritesMessageCorrectly) {
    SharedData* data = initSharedMemory();
    ASSERT_NE(data, nullptr);

    std::string testMsg = "hello shared memory";
    sendData(data, testMsg);

    EXPECT_EQ(std::string(data->msg), testMsg);
}

TEST_F(SharedMemoryTest, ReadData_ReturnsLastMessage) {
    SharedData* data = initSharedMemory();
    ASSERT_NE(data, nullptr);

    std::string testMsg = "backend test";
    sendData(data, testMsg);

    std::string readMsg = readData(data);
    EXPECT_EQ(readMsg, testMsg);
}

TEST_F(SharedMemoryTest, GetSharedMemoryStatus_ShowsUnlockedInitially) {
    SharedData* data = initSharedMemory();
    ASSERT_NE(data, nullptr);

    std::string status = getSharedMemoryStatus(data);
    EXPECT_NE(status.find("Unlocked"), std::string::npos);
}

TEST_F(SharedMemoryTest, CommunicateAtoB_TransfersMessage) {
    std::string msg = "message A to B";
    ReturnMsg result = communicateAtoB(msg);

    EXPECT_EQ(result.msg, msg);
    EXPECT_NE(result.extra, ""); // should contain mutex status
}

TEST_F(SharedMemoryTest, CommunicateBtoA_TransfersMessage) {
    std::string msg = "message B to A";
    ReturnMsg result = communicateBtoA(msg);

    EXPECT_EQ(result.msg, msg);
    EXPECT_NE(result.extra, ""); // should contain mutex status
}

TEST_F(SharedMemoryTest, MultipleConsecutiveCommunications_WorkCorrectly) {
    std::string msg = "loop test";

    for (int i = 0; i < 3; i++) {
        ReturnMsg a = communicateAtoB(msg);
        ReturnMsg b = communicateBtoA(msg);

        EXPECT_EQ(a.msg, msg);
        EXPECT_EQ(b.msg, msg);
    }
}
