#include "EventLoopThread.h"

#include <utility>

net::mrmt::EventLoopThread::EventLoopThread(std::string name)
        : m_running{false}
        , m_stop{true}
        , m_loopThreadName{std::move(name)} {

    m_eventLoop = std::make_unique<EventLoop>();
}

net::mrmt::EventLoopThread::~EventLoopThread() {
}

void net::mrmt::EventLoopThread::startLoop() {
    if (m_running) {
        return;
    }
    m_thread = std::make_unique<std::thread>(&EventLoopThread::run, this);
}

void net::mrmt::EventLoopThread::stopLoop() {
}

void net::mrmt::EventLoopThread::run() {
    while (true) {
    }
}
