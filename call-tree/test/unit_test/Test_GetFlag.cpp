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

TEST_F(CallTreeTestGetFlag, get_flag_001) {
    EXPECT_EQ(callTree->get_flag("Stack Size"), FSTACK_USAGE);
}

TEST_F(CallTreeTestGetFlag, get_flag_002) {
    EXPECT_EQ(callTree->get_flag("RTL Expand"), FDUMP_RTL_EXPAND);
}

TEST_F(CallTreeTestGetFlag, get_flag_003) {
    EXPECT_EQ(callTree->get_flag(""), FLAG_ERROR);
}