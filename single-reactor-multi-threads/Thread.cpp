#include "Thread.h"

#include <Logger.h>

static const std::string TAG{"Thread"};
net::srmt::Thread::Thread() {
    m_thread = std::make_unique<std::thread>(&Thread::run, this);
}

net::srmt::Thread::~Thread() {
    m_thread->join();
}

[[noreturn]] void net::srmt::Thread::run() {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(m_oneTimeTasksMutex);
            m_oneTimeTasksCV.wait(lock, [=] {
                return !m_oneTimeTasks.empty();
            });
        }
        auto& task = m_oneTimeTasks.front();
        {
            std::unique_lock<std::mutex> lock(m_oneTimeTasksMutex);
            m_oneTimeTasks.pop();
        }
        if (task != nullptr) {
            task();
        }
    }
}
