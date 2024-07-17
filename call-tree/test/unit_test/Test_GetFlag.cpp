#include <gtest/gtest.h>
#include <calltree.h>
#include <QApplication>

class CallTreeTest : public testing::Test {
protected:
    CallTree *callTree;

    void SetUp() override {
        callTree = new CallTree();
    }

    void TearDown() override {
        delete callTree;
    }
};

TEST_F(CallTreeTest, GetFlag_StackSize) {
    EXPECT_EQ(callTree->getFlag("Stack Size"), FSTACK_USAGE);
}

TEST_F(CallTreeTest, GetFlag_RTLExpand) {
    EXPECT_EQ(callTree->getFlag("RTL Expand"), FDUMP_RTL_EXPAND);
}

TEST_F(CallTreeTest, GetFlag_EmptyString) {
    EXPECT_EQ(callTree->getFlag(""), FLAG_ERROR);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    QApplication app(argc, argv);
    return RUN_ALL_TESTS();
}