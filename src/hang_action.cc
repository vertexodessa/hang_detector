#include "hang_detector.h"

#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

namespace HangDetector {

KillAction::KillAction(ms delay) {
    m_delay = delay;
    m_threadId = gettid();
}

CallbackAction::CallbackAction(ms delay, std::function<void(void*)> callback, void *userData) {
    m_delay = delay;
    m_callback = callback;
    m_userData = userData;
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

}
