#include "hang_detector.h"

#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <cassert>

using namespace std;
using namespace std::chrono;

namespace {
using time_point = std::chrono::time_point<std::chrono::steady_clock>;
}

namespace HangDetector {

class HangDetectorImpl {
public:
    HangDetectorImpl();
    ~HangDetectorImpl();

    void start();
    void stop();
    void restart();
    void addAction(std::shared_ptr<HangAction> a);
    void clearActions();
private:
    struct Compare {
        bool operator () (const std::shared_ptr<HangAction>& lhs, const std::shared_ptr<HangAction>& rhs) {
            return lhs->triggerTime() > rhs->triggerTime();
        }
    };
    using Actions = std::priority_queue<std::shared_ptr<HangAction>, std::vector<std::shared_ptr<HangAction>>, Compare>;
    std::thread m_thread;
    std::condition_variable m_cv;
    std::mutex m_mutex;
    Actions m_actions;
    volatile bool m_shouldQuit {false};
    bool m_started {false};

    void updateActions();
};

HangDetectorImpl::HangDetectorImpl() { }

HangDetectorImpl::~HangDetectorImpl() {
    stop();
}

void HangDetectorImpl::start() {
    m_started = true;

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

                    m_actions.pop();
                    a->update(steady_clock::now());
                    m_actions.push(a);
                    a = m_actions.top();
                }
            }
        });
}

void HangDetectorImpl::updateActions() {
    auto copy = m_actions;
    m_actions = Actions {};

    for (shared_ptr<HangAction> a = copy.top(); a->triggerTime() <= steady_clock::now(); ) {
        a->update(steady_clock::now());
        a = copy.top();
        copy.pop();
        m_actions.push(a);
    }
}

void HangDetectorImpl::restart() {
    unique_lock<mutex> lock(m_mutex);
    updateActions();
}

void HangDetectorImpl::stop() {
    if (!m_started)
        return;

    {
        unique_lock<mutex> lock(m_mutex);
        m_shouldQuit = true;
        m_cv.notify_one();
    }
    m_thread.join();
    m_started = false;
}

void HangDetectorImpl::addAction(shared_ptr<HangAction> a) {
    unique_lock<mutex> lock(m_mutex);
    a->update(steady_clock::now());
    m_actions.push(a);
    m_cv.notify_one();
}

void HangDetectorImpl::clearActions() {
    unique_lock<mutex> lock(m_mutex);
    m_actions = Actions {};
    m_cv.notify_one();
}

Detector::Detector()  : m_impl(new HangDetectorImpl()) {
}

Detector::~Detector() {
}

void Detector::start() {
    m_impl->start();
}

void Detector::restart() {
    m_impl->restart();
}

void Detector::stop() {
    m_impl->stop();
}

void Detector::addAction(shared_ptr<HangAction> a) {
    m_impl->addAction(a);
}

void Detector::clearActions() {
    m_impl->clearActions();
}



}
