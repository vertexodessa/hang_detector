#include "hang_detector.h"

#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <cassert>

using namespace std;
using namespace std::chrono;

namespace HangDetector {

HangDetector::HangAction::HangAction(ms delay, int threadId) {
    m_delay = delay;
    m_threadId = threadId;
}

HangDetector::HangAction::HangAction(ms delay, std::function<void(void*)> callback, void *userData) {
    m_delay = delay;
    m_callback = callback;
    m_userData = userData;
}

HangDetector::HangDetector() {
}

HangDetector::~HangDetector() {
    stop();
}

void HangDetector::start(milliseconds interval) {
    m_interval = interval;
    time_point<steady_clock> triggerTime = time_point<steady_clock>::max();
    if (!m_actions.empty()) {
        triggerTime = m_actions.top()->m_triggerTime;
    }

    m_thread = thread( [&, this] () {
            while (!m_shouldQuit) {
                unique_lock<mutex> lock(m_mutex);
                m_cv.wait_until(lock, triggerTime);

                if (m_actions.empty())
                    continue;

                auto copy = m_actions;
                for (shared_ptr<HangAction> a = copy.top(); a->m_triggerTime <= steady_clock::now(); ) {
                    a->execute();

                    a = copy.top();
                    copy.pop();
                }
            }
        });
}

void HangDetector::updateActions() {
    auto copy = m_actions;
    for (shared_ptr<HangAction> a = copy.top(); a->m_triggerTime <= steady_clock::now(); ) {
        a->update(steady_clock::now());
        a = copy.top();
        copy.pop();
    }
}


void HangDetector::restart() {
    unique_lock<mutex> lock(m_mutex);
    updateActions();
}

void HangDetector::stop() {
    unique_lock<mutex> lock(m_mutex);
    m_shouldQuit = true;
    m_cv.notify_one();
    m_thread.join();
}

void HangDetector::addAction(shared_ptr<HangAction> a) {
    unique_lock<mutex> lock(m_mutex);
    m_actions.push(a);
    m_cv.notify_one();
}

void HangDetector::clearActions() {
    unique_lock<mutex> lock(m_mutex);
    while(!m_actions.empty())
        m_actions.pop();
    m_cv.notify_one();
}

}
