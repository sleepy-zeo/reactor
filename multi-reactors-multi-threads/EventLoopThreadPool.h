#ifndef REACTOR_MULTI_REACTORS_MULTI_THREADS_EVENTLOOPTHREADPOOL_H_
#define REACTOR_MULTI_REACTORS_MULTI_THREADS_EVENTLOOPTHREADPOOL_H_

#include "EventLoop.h"
#include "EventLoopThread.h"

namespace net {
namespace mrmt {

class EventLoopThreadPool {
public:
    EventLoopThreadPool(int threadsCount, std::string poolName = "");
    ~EventLoopThreadPool();

    void start();
    void stop();
    std::shared_ptr<EventLoop> getNextLoop();

private:
    std::mutex m_eventLoopThreadPoolStartedMutex;
    bool m_eventLoopThreadPoolStarted;

    std::mutex m_nextMutex;
    int m_next;

    std::deque<std::shared_ptr<EventLoopThread>> m_eventLoopThreadsList;

    int m_threadCount;

    std::string m_poolName;
};

}  // namespace mrmt
}  // namespace net

#endif  // REACTOR_MULTI_REACTORS_MULTI_THREADS_EVENTLOOPTHREADPOOL_H_
