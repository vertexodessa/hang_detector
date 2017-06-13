#ifndef HANGDETECTOR_H
#define HANGDETECTOR_H

#include "hang_action.h"

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
    // TODO: move the data to pimpl
    ms m_interval {1000};
    std::thread m_thread;
    std::condition_variable m_cv;
    std::mutex m_mutex;
    std::priority_queue<std::shared_ptr<HangAction>> m_actions;
    bool m_shouldQuit {false};

    void updateActions();
};

}
#endif
