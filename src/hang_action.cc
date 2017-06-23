#include "hang_detector.h"

// FIXME: make Breakpad integration optional (build define)
#include <client/linux/handler/exception_handler.h>

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

WriteMinidumpAction::WriteMinidumpAction(ms delay, const std::string path, int signal, std::condition_variable* cv) {
    m_delay = delay;
    m_signal = signal;
    m_cv = cv;
    m_path = path;
}

void HangAction::update (time_point now) {
    m_triggerTime = now + m_delay;
}

void KillAction::execute() {
    printf("Kill action executed. killing PID %d\n", m_threadId);
    kill(m_threadId, 6);
}

void WriteMinidumpAction::execute() {
    using namespace google_breakpad;
    static MinidumpDescriptor descriptor(m_path.c_str());

    auto callback = [](const MinidumpDescriptor& /*descriptor*/,
                       void* /*context*/,
                       bool succeeded) -> bool {
        log("callback!\n");
        return succeeded;
    };

    ExceptionHandler eh(descriptor, nullptr, callback, nullptr, true, -1);

    log("Child %d is going to crash with signal %d\n", getpid(), m_signal);
    eh.WriteMinidump();

    if(m_cv)
        m_cv->notify_one();
}

}
