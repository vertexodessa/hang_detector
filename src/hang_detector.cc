#include "hang_detector.h"

#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <cassert>

using namespace std;
using namespace std::chrono;

namespace HangDetector {

HangDetector::HangDetector() {
}

HangDetector::~HangDetector() {
    stop();
}

void HangDetector::start(milliseconds interval) {
    m_interval = interval;
    time_point triggerTime = time_point::max();

    m_thread = thread( [&, this] () {
            while (!m_shouldQuit) {
                if (!m_actions.empty()) {
                    triggerTime = m_actions.top()->triggerTime();
                } else {
                    triggerTime = time_point::max();
                }

                unique_lock<mutex> lock(m_mutex);
                m_cv.wait_until(lock, triggerTime);

                if (m_actions.empty())
                    continue;
                if(m_shouldQuit)
                    return;

                shared_ptr<HangAction> a = m_actions.top();
                while (a->triggerTime() <= steady_clock::now()) {
                    a->execute();
                    a->update(steady_clock::now());

                    m_actions.pop();
                    m_actions.push(a);
                    a = m_actions.top();
                }
            }
        });
}

void HangDetector::updateActions() {
    auto copy = m_actions;
    m_actions = std::priority_queue<std::shared_ptr<HangAction>> {};

    for (shared_ptr<HangAction> a = copy.top(); a->triggerTime() <= steady_clock::now(); ) {
        a->update(steady_clock::now());
        a = copy.top();
        copy.pop();
        m_actions.push(a);
    }
}

void HangDetector::restart() {
    unique_lock<mutex> lock(m_mutex);
    updateActions();
}

void HangDetector::stop() {
    {
        unique_lock<mutex> lock(m_mutex);
        m_shouldQuit = true;
        m_cv.notify_one();
    }
    m_thread.join();
}

void HangDetector::addAction(shared_ptr<HangAction> a) {
    unique_lock<mutex> lock(m_mutex);
    m_actions.push(a);
    a->update(steady_clock::now());
    m_cv.notify_one();
}

void HangDetector::clearActions() {
    unique_lock<mutex> lock(m_mutex);
    m_actions = std::priority_queue<std::shared_ptr<HangAction>> {};
    m_cv.notify_one();
}

}
