#ifndef REACTOR_MULTI_REACTORS_MULTI_THREADS_EVENTLOOPTHREAD_H_
#define REACTOR_MULTI_REACTORS_MULTI_THREADS_EVENTLOOPTHREAD_H_

#include "EventLoop.h"
#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace net {
namespace mrmt {

class EventLoopThread {

public:
    explicit EventLoopThread(std::string name = "");
    ~EventLoopThread();

    void startLoop();
    void stopLoop();

    std::shared_ptr<EventLoop> getEventLoop();

private:
    void run();

    bool m_started;
    std::mutex m_startedMutex;

    std::string m_loopThreadName;
    std::unique_ptr<std::thread> m_thread;

    std::mutex m_loopMutex;
    std::condition_variable m_loopCV;
    std::shared_ptr<EventLoop> m_eventLoop;
};

}  // namespace mrmt
}  // namespace net

#endif  // REACTOR_MULTI_REACTORS_MULTI_THREADS_EVENTLOOPTHREAD_H_
