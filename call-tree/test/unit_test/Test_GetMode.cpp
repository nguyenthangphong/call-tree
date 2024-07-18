#include <gtest/gtest.h>
#include <calltree.h>

class CallTreeTestGetMode : public ::testing::Test {
protected:
    CallTree *callTree;

    void SetUp() override {
        callTree = new CallTree();
    }

    void TearDown() override {
        delete callTree;
    }
};

TEST_F(CallTreeTestGetMode, GetMode_001) {
    EXPECT_EQ(callTree->getMode("Build"), BUILD);
}

TEST_F(CallTreeTestGetMode, GetMode_002) {
    EXPECT_EQ(callTree->getMode("Run"), RUN);
}

TEST_F(CallTreeTestGetMode, GetMode_003) {
    EXPECT_EQ(callTree->getMode(""), MODE_ERROR);
}