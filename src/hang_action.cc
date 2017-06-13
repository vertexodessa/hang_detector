#include "hang_detector.h"

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

}
