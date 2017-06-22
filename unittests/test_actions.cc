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
    // FIXME: figure out how to test this correctly.

    // log("About to start KillTest on %d\n", getpid());
    // EXPECT_DEATH([](){
    //         Detector hd;
    //         hd.addAction(make_shared<KillAction>(ms(100)));
    //         hd.start();
    //         this_thread::sleep_for(ms(5000));
    //     }(), ".*");
}

TEST(ActionsTest, DiedAndGeneratedMinidump) {
    // install breakpad handlers
    // setup a CallbackAction to fork and kill the child with timeout = 1.5 sec
    // EXPECT_DEATH with a nullptr
    // stop detector
    // teardown breakpad
    // check that the minidump was created
}

struct Data {
    int status {-1};
    condition_variable cv;
};

static void forkAndCrash(void* pData) {
    Data& data = *(Data*)pData;
    log("Callback fired in ForkTest on %d\n", getpid());

    int pid = fork();
    if (pid == 0) {
        // crash
        log("Child %d is going to crash\n", getpid());
        kill(6, getpid());
        exit(0);
    } else if (pid > 0) {
        log("Parent %d\n", getpid());
        // wait until child crashes and generates minidump
        ::waitpid(pid, &data.status, 0);
        data.cv.notify_one();
        log("Parent %d wait finished\n", getpid());
    } else {
        log("ERROR: fork finished unsuccessfully\n");
    }
};

TEST(ActionsTest, ForkedAndGeneratedMinidump) {
    // install breakpad handlers
    using namespace google_breakpad;
    MinidumpDescriptor descriptor("./");
    ExceptionHandler eh(descriptor, nullptr, nullptr, nullptr, true, -1);

    Timer t;
    // setup a CallbackAction to fork and kill the child with timeout
    Detector hd;
    Data status;
    hd.addAction(make_shared<CallbackAction>(ms(500), forkAndCrash, &status));
    hd.start();

    // wait for test to finish
    mutex m;
    unique_lock<mutex> lock(m);
    status.cv.wait(lock);

    EXPECT_LE(t.elapsed(), ms(1000));
    EXPECT_GT(t.elapsed(), ms(100));

    EXPECT_EQ(status.status, 0);
    // stop detector
    hd.stop();
}

TEST(HangDetectorTest, NotStartedDoesNotHangOrDie) {
    Detector hd;
}


