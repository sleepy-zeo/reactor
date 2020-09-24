#include "EventLoopThread.h"

#include <utility>

net::mrmt::EventLoopThread::EventLoopThread(std::string name)
        : m_started{false}
        , m_loopThreadName{std::move(name)}
        , m_eventLoop{nullptr} {
}

net::mrmt::EventLoopThread::~EventLoopThread() {
    stopLoop();
}

void net::mrmt::EventLoopThread::startLoop() {
    if (m_started) {
        return;
    }
    m_thread = std::make_unique<std::thread>(&EventLoopThread::run, this);
    {
        std::unique_lock<std::mutex> lock(m_loopMutex);
        m_loopCV.wait(lock, [this] {
            return m_eventLoop != nullptr;
        });
    }
    {
        std::lock_guard<std::mutex> lock_guard(m_startedMutex);
        m_started = true;
    }
}

void net::mrmt::EventLoopThread::stopLoop() {
    if (!m_started) {
        return;
    }
    if (m_eventLoop != nullptr) {
        m_eventLoop->quit();
    }
    if (m_thread->joinable())
        m_thread->join();
    {
        std::unique_lock<std::mutex> lock(m_startedMutex);
        m_started = false;
    }
}

std::shared_ptr<net::mrmt::EventLoop> net::mrmt::EventLoopThread::getEventLoop() {
    return m_eventLoop;
}

void net::mrmt::EventLoopThread::run() {
    auto* loop = new EventLoop();
    {
        std::unique_lock<std::mutex> lock(m_loopMutex);
        m_eventLoop.reset(loop);
        m_loopCV.notify_one();
    }
    m_eventLoop->loop();
    m_eventLoop.reset();
    m_eventLoop = nullptr;
}
