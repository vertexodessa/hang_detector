#include "hang_detector.h"

#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>

#define gettid() syscall(SYS_gettid)
#define log(...)

namespace HangDetector {

namespace Utils {
void forkAndCrash(void* pData) {
    ForkAndCrashData& data = *(ForkAndCrashData*)pData;
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
        waitpid(pid, &data.status, 0);
        data.cv.notify_one();
        log("Parent %d wait finished\n", getpid());
    } else {
        log("ERROR: fork finished unsuccessfully\n");
    }
}
}

KillAction::KillAction(ms delay) {
    m_delay = delay;
    m_threadId = gettid();
}

CallbackAction::CallbackAction(ms delay, std::function<void(void*)> callback, void *userData) {
    m_delay = delay;
    m_callback = callback;
    m_userData = userData;
}

ForkAndKillAction::ForkAndKillAction(ms delay, int signal, Utils::ForkAndCrashData* data) {
    m_delay = delay;
    m_signal = signal;
    m_data = data;
}

void HangAction::update (time_point now) {
    m_triggerTime = now + m_delay;
}

void KillAction::execute() {
    printf("Kill action executed. killing PID %d\n", m_threadId);
    kill(m_threadId, 6);
}

void CallbackAction::execute() {
    if(m_callback)
        m_callback(m_userData);
}

void ForkAndKillAction::execute() {
    log("ForkAndKillAction::execute %d\n", getpid());

    int pid = fork();
    if (pid == 0) {
        // crash
        log("Child %d is going to crash\n", getpid());
        kill(m_signal, getpid());
        exit(0);
    } else if (pid > 0) {
        log("Parent %d\n", getpid());
        // wait until child crashes and generates minidump
        int status;
        waitpid(pid, &status, 0);
        if (m_data) {
            m_data->status = status;
            m_data->cv.notify_one();
        }
        log("Parent %d wait finished\n", getpid());
    } else {
        log("ERROR: fork finished unsuccessfully\n");
    }

}

}
