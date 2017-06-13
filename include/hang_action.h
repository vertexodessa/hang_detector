#ifndef HANG_ACTION
#define HANG_ACTION

#include <condition_variable>
#include <functional>
#include <thread>
#include <chrono>
#include <queue>

namespace HangDetector {

using ms = std::chrono::milliseconds;
using time_point = std::chrono::time_point<std::chrono::steady_clock>;

class HangAction {
public:
    virtual void execute() = 0;
    virtual time_point triggerTime() const { return m_triggerTime; }
    virtual void update(time_point from);
private:
    time_point  m_triggerTime {time_point::max()};
protected:
    ms          m_delay       {30000};
};

class KillHangAction : public HangAction {
public:
    KillHangAction(ms delay, int threadId);
    void execute() final {};
private:
    int  m_threadId  {-1};
};

class CallbackHangAction : public HangAction {
public:
    CallbackHangAction(ms delay, std::function<void(void*)> callback, void *userData);
    void execute() final {};
private:
    std::function<void(void*)> m_callback    {nullptr};
    void*                      m_userData    {nullptr};
};

inline bool operator <(const std::shared_ptr<HangAction>& lhs, const std::shared_ptr<HangAction>& rhs) {
    return lhs->triggerTime() < rhs->triggerTime();
}

}

#endif
