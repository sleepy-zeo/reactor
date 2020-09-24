#include "EventLoopThreadPool.h"

net::mrmt::EventLoopThreadPool::EventLoopThreadPool(int threadsCount, std::string poolName)
        : m_poolName{std::move(poolName)}
        , m_threadCount{threadsCount}
        , m_eventLoopThreadPoolStarted{false} {
}

net::mrmt::EventLoopThreadPool::~EventLoopThreadPool() {
    stop();
}

void net::mrmt::EventLoopThreadPool::start() {
    if (m_eventLoopThreadPoolStarted) {
        return;
    }
    for (int i = 0; i < m_threadCount; ++i) {
        std::shared_ptr<EventLoopThread> loopThread(new EventLoopThread);
        loopThread->startLoop();
        m_eventLoopThreadsList.push_back(loopThread);
    }
    {
        std::lock_guard<std::mutex> lock_guard(m_eventLoopThreadPoolStartedMutex);
        m_eventLoopThreadPoolStarted = true;
    }
}

void net::mrmt::EventLoopThreadPool::stop() {
    if (!m_eventLoopThreadPoolStarted) {
        return;
    }
    for (auto iter = m_eventLoopThreadsList.begin(); iter != m_eventLoopThreadsList.end(); ++iter) {
        (*iter)->stopLoop();
    }
    {
        std::lock_guard<std::mutex> lock_guard(m_eventLoopThreadPoolStartedMutex);
        m_eventLoopThreadPoolStarted = false;
    }
}

std::shared_ptr<net::mrmt::EventLoop> net::mrmt::EventLoopThreadPool::getNextLoop() {
    if (!m_eventLoopThreadPoolStarted) {
        return nullptr;
    }
    std::shared_ptr<EventLoop> loop = nullptr;
    if (!m_eventLoopThreadsList.empty()) {
        std::lock_guard<std::mutex> lock_guard(m_nextMutex);
        loop = m_eventLoopThreadsList[m_next]->getEventLoop();
        ++m_next;
        if (m_next >= m_threadCount) {
            m_next = 0;
        }
    }
    return loop;
}
