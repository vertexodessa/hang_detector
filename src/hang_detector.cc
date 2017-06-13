#include "hang_detector.h"

#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <cassert>

using namespace std;
using namespace std::chrono;

namespace {
atomic<int> g_instanceCount {0};
mutex g_mutex;
void initialize() {
    unique_lock<mutex> lock(g_mutex);
};
void shutdown() {
    unique_lock<mutex> lock(g_mutex);
};
}

namespace HangDetector {

HangDetector::HangAction::HangAction(ms delay, int threadId) {
    
}

HangDetector::HangAction::HangAction(ms delay, std::function<void(void*)> callback, void *userData) {
    
}
    //   private:
    //     int                        m_delay     {30000};
    //     std::function<void(void*)> m_callback  {nullptr};
    //     void*                      m_userData  {nullptr};
    // };

HangDetector::HangDetector() {
    if (g_instanceCount.fetch_add(1) == 1) {
        initialize();
    }
}

HangDetector::~HangDetector() {
    if (g_instanceCount.fetch_sub(1) == 1) {
        shutdown();
    }
}

void HangDetector::start(milliseconds interval) {
    m_interval = interval;
    time_point<steady_clock> triggerTime = time_point<steady_clock>::max();
    if (!m_actions.empty()) {
        triggerTime = m_actions.top().m_triggerTime;
    }

    m_thread = thread( [&, this] () {
            while (!m_shouldQuit) {
                unique_lock<mutex> lock(g_mutex);
                m_cv.wait_until(lock, triggerTime);
                // check actions queue and fire expired actions
                auto copy = m_actions;
                for (HangAction a = copy.top(); a.m_triggerTime <= steady_clock::now(); ) {
                    a.execute();
                    a = copy.top();
                    copy.pop();
                }
            }
        });
}

void HangDetector::updateActions() {
    
}


void HangDetector::restart() {
    unique_lock<mutex> lock(g_mutex);
    updateActions();
}

void HangDetector::stop() {
    unique_lock<mutex> lock(g_mutex);
    m_shouldQuit = true;
    m_cv.notify_one();
}

void HangDetector::addAction(HangAction a) {
    unique_lock<mutex> lock(g_mutex);
    m_actions.push(a);
    m_cv.notify_one();
}

void HangDetector::clearActions() {
    // clear m_nextAction
}

}
//   private:
//     int m_interval {1000};
// };
