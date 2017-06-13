#include "hang_detector.h"

#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <cassert>

using namespace std;
using namespace std::chrono;

namespace HangDetector {

KillHangAction::KillHangAction(ms delay, int threadId) {
    m_delay = delay;
    m_threadId = threadId;
}

CallbackHangAction::CallbackHangAction(ms delay, std::function<void(void*)> callback, void *userData) {
    m_delay = delay;
    m_callback = callback;
    m_userData = userData;
}

}
