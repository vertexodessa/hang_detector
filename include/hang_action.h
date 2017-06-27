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

// This action is useful if you're trying to investigate application performance issues.
// Install this action, investigate backtrace of the hang in the generated minidump.
class WriteMinidumpAction : public HangAction {
public:
    WriteMinidumpAction(ms delay, const std::string path = "./", std::condition_variable* cv = nullptr);
    void execute() final;
private:
    std::string m_path;
    std::condition_variable* m_cv;
};

template <class T>
class CallbackAction : public HangAction {
public:
    CallbackAction(ms delay, std::function<void(T*)> callback, T *userData) {
        m_delay = delay;
        m_callback = callback;
        m_userData = userData;
    }
    void execute() final {
        if(m_callback)
            m_callback(m_userData);
    }
private:
    std::function<void(T*)> m_callback    {nullptr};
    T*                      m_userData    {nullptr};
};

}

#endif
