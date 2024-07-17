#include <gtest/gtest.h>
#include <calltree.h>

class CallTreeTestGetFlag : public ::testing::Test {
protected:
    CallTree *callTree;

    void SetUp() override {
        callTree = new CallTree();
    }

    void TearDown() override {
        delete callTree;
    }
};

TEST_F(CallTreeTestGetFlag, GetFlag_StackSize) {
    EXPECT_EQ(callTree->getFlag("Stack Size"), FSTACK_USAGE);
}

TEST_F(CallTreeTestGetFlag, GetFlag_RTLExpand) {
    EXPECT_EQ(callTree->getFlag("RTL Expand"), FDUMP_RTL_EXPAND);
}

TEST_F(CallTreeTestGetFlag, GetFlag_EmptyString) {
    EXPECT_EQ(callTree->getFlag(""), FLAG_ERROR);
}