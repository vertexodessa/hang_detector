#include "hang_detector.h"

#include <client/linux/handler/exception_handler.h>
#include <gtest/gtest.h>

#include <chrono>

//#define log(...) printf(__VA_ARGS__)
#define log(...)

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

TEST(ActionsTest, RestartWorks) {
    Detector hd;
    volatile bool triggered = false;
    hd.addAction(make_shared<CallbackAction<volatile bool>>(ms(500), [](volatile bool* arg){
                *arg = true;
            }, &triggered));
    hd.start();

    this_thread::sleep_for(ms(300));
    hd.restart();

    this_thread::sleep_for(ms(300));
    EXPECT_EQ(triggered, false);
}

TEST(ActionsTest, 1SecActionWasTriggeredOnce) {
    Detector hd;
    volatile int count = 0;
    hd.addAction(make_shared<CallbackAction<volatile int>>(ms(950), [](volatile int* arg){
                (*arg)++;
            }, &count));
    hd.start();
    this_thread::sleep_for(ms(1000));
    EXPECT_EQ(count, 1);
}

TEST(ActionsTest, 500MSecActionWasTriggeredOnce) {
    Detector hd;
    volatile int count = 0;
    hd.addAction(make_shared<CallbackAction<volatile int>>(ms(500), [](volatile int* arg){
                (*arg)++;
            }, &count));
    hd.start();
    this_thread::sleep_for(ms(700));
    EXPECT_EQ(count, 1);
}

TEST(ActionsTest, Three950MsActionsAfter5Seconds) {
    Detector hd;
    volatile int count = 0;
    hd.addAction(make_shared<CallbackAction<volatile int>>(ms(950), [](volatile int* count){
                (*count)++;
            }, &count));
    hd.start();
    this_thread::sleep_for(ms(3000));
    EXPECT_EQ(count, 3);
}

TEST(ActionsTest, WriteMinidumpAction) {
    Timer t;
    // setup a CallbackAction to fork and kill the child with timeout
    Detector hd;
    condition_variable cv;
    hd.addAction(make_shared<WriteMinidumpAction>(ms(200), "./", 6, &cv));
    hd.start();

    // wait for test to finish
    mutex m;
    unique_lock<mutex> lock(m);
    cv.wait_for(lock, ms(5000));

    // stop detector
    hd.stop();

    EXPECT_LE(duration_cast<milliseconds>(t.elapsed()).count(), 1000);
    EXPECT_GT(duration_cast<milliseconds>(t.elapsed()).count(), 100);
}

TEST(ActionsTest, KilledOnTimeout) {
    // FIXME: figure out how to test this correctly.

    // log("About to start KillTest on %d\n", getpid());
    // EXPECT_DEATH([](){
    //         Detector hd;
    //         hd.addAction(make_shared<KillAction>(ms(100)));
    //         hd.start();
    //         this_thread::sleep_for(ms(5000));
    //     }(), ".*");
}

TEST(BreakpadTest, TwoHandlers) {
    // FIXME: make sure it's possible to create two minidump handlers in a single app
}

TEST(HangDetectorTest, NotStartedDoesNotHangOrDie) {
    Detector hd;
}


