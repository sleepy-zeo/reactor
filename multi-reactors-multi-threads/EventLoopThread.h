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
    EventLoopThread(std::string  name="");
    ~EventLoopThread();

    void startLoop();
    void stopLoop();


private:
    void run();

    bool m_stop;

    bool m_running;
    std::mutex m_runningMutex;
    std::condition_variable m_runningCV;

    std::string m_loopThreadName;
    std::unique_ptr<std::thread> m_thread;
    std::unique_ptr<EventLoop> m_eventLoop;

};

}  // namespace mrmt
}  // namespace net

#endif  // REACTOR_MULTI_REACTORS_MULTI_THREADS_EVENTLOOPTHREAD_H_
