#include "hang_detector.h"

#include <gtest/gtest.h>

#include <chrono>

using namespace HangDetector;
using namespace std;
using namespace std::chrono;

class Timer {
public:
    Timer() { m_started = steady_clock::now(); }
    nanoseconds elapsed() { return steady_clock::now() - m_started; }
private:
    chrono::time_point<steady_clock> m_started;
};

// class ActionTest : public ::testing::Test {
// protected:
//     void SetUp() override { }
//     void TearDown() override { }
// };

TEST(ActionsTest, 1SecActionWasTriggeredOnce) {
    Detector hd;
    int count = 0;
    hd.addAction(make_shared<CallbackAction>(ms(950), [](void* count_){
                int* count = static_cast<int*>(count_);
                (*count)++;
            }, &count));
    hd.start();
    this_thread::sleep_for(ms(1000));
    EXPECT_EQ(count, 1);
}

TEST(ActionsTest, 500MSecActionWasTriggeredOnce) {
    Detector hd;
    int count = 0;
    hd.addAction(make_shared<CallbackAction>(ms(500), [](void* count_){
                int* count = static_cast<int*>(count_);
                (*count)++;
            }, &count));
    hd.start();
    this_thread::sleep_for(ms(700));
    EXPECT_EQ(count, 1);
}

TEST(ActionsTest, Five950MsActionsAfter5Seconds) {
    Detector hd;
    int count = 0;
    hd.addAction(make_shared<CallbackAction>(ms(950), [](void* count_){
                int* count = static_cast<int*>(count_);
                (*count)++;
            }, &count));
    hd.start();
    this_thread::sleep_for(ms(5000));
    EXPECT_EQ(count, 5);
}

TEST(ActionsTest, KilledOnTimeout) {
    printf("About to start KillTest on %d\n", getpid());
    EXPECT_DEATH([](){
            Detector hd;
            hd.addAction(make_shared<KillAction>(ms(100)));
            hd.start();
            this_thread::sleep_for(ms(5000));
        }(), ".*");
}

TEST(ActionsTest, DiedAndGeneratedMinidump) {
    // install breakpad handlers
    // setup a CallbackAction to fork and kill the child with timeout = 1.5 sec
    // EXPECT_DEATH with a nullptr
    // stop detector
    // teardown breakpad
    // check that the minidump was created
}

TEST(ActionsTest, ForkedAndGeneratedMinidump) {
    // FIXME: breakpad dependency (build and install).
    
    // install breakpad handlers
    // setup a CallbackAction to fork and kill the child with timeout = 1.5 sec
    // sleep for 2 seconds
    // stop detector
    // teardown breakpad
    // check that the minidump was created
}
