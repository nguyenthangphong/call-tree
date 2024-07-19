#include <gtest/gtest.h>
#include <calltree.h>

class CallTreeTestRun : public ::testing::Test {
protected:
    CallTree *callTree;

    void SetUp() override {
        callTree = new CallTree();
    }

    void TearDown() override {
        delete callTree;
    }
};

#ifdef Q_OS_LINUX
TEST_F(CallTreeTestRun, run_001) {
    EXPECT_EQ(callTree->run(FSTACK_USAGE, "/home/phong/Desktop/training_ut_it/rcar-env/r_impdrv_osdep_sil.su"), STATUS_OK);
}

TEST_F(CallTreeTestRun, run_002) {
    EXPECT_EQ(callTree->run(FDUMP_RTL_EXPAND, "/home/phong/Desktop/training_ut_it/rcar-env/r_impdrv_api.c.245r.expand"), STATUS_OK);
}

TEST_F(CallTreeTestRun, run_003) {
    EXPECT_EQ(callTree->run(FSTACK_USAGE, "/home/phong/Desktop/training_ut_it/rcar-env/r_impdrv_osdep_sil"), STATUS_ERROR);
}

TEST_F(CallTreeTestRun, run_004) {
    EXPECT_EQ(callTree->run(FDUMP_RTL_EXPAND, "/home/phong/Desktop/training_ut_it/rcar-env/r_impdrv_api.c.245r"), STATUS_ERROR);
}
#elif defined(Q_OS_WIN)
TEST_F(CallTreeTestRun, run_001) {
    EXPECT_EQ(callTree->run(FSTACK_USAGE, "/home/phong/Desktop/training_ut_it/rcar-env/r_impdrv_osdep_sil.su"), STATUS_OK);
}

TEST_F(CallTreeTestRun, run_002) {
    EXPECT_EQ(callTree->run(FDUMP_RTL_EXPAND, "/home/phong/Desktop/training_ut_it/rcar-env/r_impdrv_api.c.245r.expand"), STATUS_OK);
}

TEST_F(CallTreeTestRun, run_003) {
    EXPECT_EQ(callTree->run(FSTACK_USAGE, "/home/phong/Desktop/training_ut_it/rcar-env/r_impdrv_osdep_sil"), STATUS_ERROR);
}

TEST_F(CallTreeTestRun, run_004) {
    EXPECT_EQ(callTree->run(FDUMP_RTL_EXPAND, "/home/phong/Desktop/training_ut_it/rcar-env/r_impdrv_api.c.245r"), STATUS_ERROR);
}
#endif