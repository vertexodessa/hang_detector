#ifndef HANG_ACTION
#define HANG_ACTION

#include <functional>
#include <thread>
#include <chrono>
#include <condition_variable>

namespace HangDetector {

using ms = std::chrono::milliseconds;
using time_point = std::chrono::time_point<std::chrono::steady_clock>;

namespace Utils {
struct ForkAndCrashData {
    int status {-1};
    std::condition_variable cv;
};
void forkAndCrash(void* pData);
}

class HangAction {
public:
    virtual void execute() = 0;
    virtual time_point triggerTime() const { return m_triggerTime; }
    virtual void update(time_point from);

    HangAction()                  = default;
    virtual ~HangAction()         = default;

    HangAction(HangAction&&)      = delete;
    HangAction(const HangAction&) = delete;
private:
    time_point  m_triggerTime {time_point::max()};
protected:
    ms          m_delay       {30000};
};

class KillAction : public HangAction {
public:
    KillAction(ms delay);
    void execute() final;
private:
    pid_t  m_threadId  {-1};
};

class CallbackAction : public HangAction {
public:
    CallbackAction(ms delay, std::function<void(void*)> callback, void *userData);
    void execute() final;
private:
    std::function<void(void*)> m_callback    {nullptr};
    void*                      m_userData    {nullptr};
};

// This action is useful if you're trying to investigate application performance issues.
// Enable the core generation, install this action, investigate backtrace of the hang.
class ForkAndKillAction : public HangAction {
public:
    ForkAndKillAction(ms delay, int signal = 6, Utils::ForkAndCrashData* data = nullptr);
    void execute() final;
private:
    int m_signal;
    std::condition_variable m_cv;
    Utils::ForkAndCrashData* m_data;
};

}

#endif
