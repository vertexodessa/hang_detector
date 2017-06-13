#ifndef HANGDETECTOR_H
#define HANGDETECTOR_H

#include <condition_variable>
#include <functional>
#include <thread>
#include <chrono>
#include <queue>

namespace HangDetector {

class HangDetector {
    using ms = std::chrono::milliseconds;
public:
    class HangAction {
        using time_point = std::chrono::time_point<std::chrono::steady_clock>;
    public:
        HangAction(ms delay, int threadId);
        HangAction(ms delay, std::function<void(void*)> callback, void *userData);
        void execute();
    private:
        friend class HangDetector;
        friend bool operator < (const HangDetector::HangAction& lhs, const HangDetector::HangAction& rhs);

        ms                         m_delay      {30000};
        std::function<void(void*)> m_callback   {nullptr};
        void*                      m_userData   {nullptr};
        time_point                 m_triggerTime;

        time_point triggerTime();
        void update(time_point from);
    };

    HangDetector();
    ~HangDetector();

    void start(ms interval = ms(1000));
    void stop();
    void restart();
    void addAction(HangAction a);
    void clearActions();
private:
    // TODO: move the data to pimpl
    ms m_interval {1000};
    std::thread m_thread;
    std::condition_variable m_cv;
    std::priority_queue<HangAction> m_actions;
    bool m_shouldQuit {false};

    void updateActions();
};

bool operator <(const HangDetector::HangAction& lhs, const HangDetector::HangAction& rhs) {
    return lhs.m_triggerTime < rhs.m_triggerTime;
}

}
#endif
