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

TEST_F(CallTreeTestGetMode, GetMode_Build) {
    EXPECT_EQ(callTree->getMode("Build"), BUILD);
}

TEST_F(CallTreeTestGetMode, GetMode_Run) {
    EXPECT_EQ(callTree->getMode("Run"), RUN);
}

TEST_F(CallTreeTestGetMode, GetMode_Error) {
    EXPECT_EQ(callTree->getMode(""), MODE_ERROR);
}