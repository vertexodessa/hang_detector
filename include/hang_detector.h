#ifndef HANGDETECTOR_H
#define HANGDETECTOR_H

#include "hang_action.h"

#include <condition_variable>
#include <queue>

namespace HangDetector {

using ms = std::chrono::milliseconds;
using time_point = std::chrono::time_point<std::chrono::steady_clock>;

class HangDetector {
public:
    HangDetector();
    ~HangDetector();

    void start(ms interval = ms(1000));
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
    // TODO: move the data to pimpl
    ms m_interval {1000};
    std::thread m_thread;
    std::condition_variable m_cv;
    std::mutex m_mutex;
    Actions m_actions;
    volatile bool m_shouldQuit {false};

    void updateActions();
};

}
#endif
