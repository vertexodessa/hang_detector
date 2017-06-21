#include "hang_detector.h"

#include <gtest/gtest.h>

using namespace HangDetector;
using namespace std;

class ActionTest : public ::testing::Test {
protected:
    void SetUp() override { }
    void TearDown() override { }
};

TEST_F(ActionTest, 1SecActionWasTriggered) {
    
}

TEST_F(ActionTest, 500MSecActionWasTriggered) {
    
}

TEST_F(ActionTest, Five1SecActionsAfter5Seconds) {
    
}

TEST_F(ActionTest, KilledOnTimeout) {
    EXPECT_DEATH([](){
            Detector hd;
            hd.addAction(make_shared<KillAction>(ms(100)));
            hd.start();
            sleep(5);
        }(), ".*");
}

TEST_F(ActionTest, ForkedAndGeneratedMinidump) {
    
}
