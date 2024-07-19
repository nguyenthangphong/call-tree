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

TEST_F(CallTreeTestGetMode, get_mode_001) {
    EXPECT_EQ(callTree->get_mode("Build"), BUILD);
}

TEST_F(CallTreeTestGetMode, get_mode_002) {
    EXPECT_EQ(callTree->get_mode("Run"), RUN);
}

TEST_F(CallTreeTestGetMode, get_mode_003) {
    EXPECT_EQ(callTree->get_mode(""), MODE_ERROR);
}