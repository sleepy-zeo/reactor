#ifndef REACTOR_SINGLE_REACTOR_MULTI_THREADS_THREAD_H_
#define REACTOR_SINGLE_REACTOR_MULTI_THREADS_THREAD_H_

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace net {
namespace srmt {

class Thread {

public:
    Thread();
    ~Thread();

    [[noreturn]] void run();

    template <typename Task, typename... Args>
    void addTask(Task task, Args&&... args);

private:
    std::unique_ptr<std::thread> m_thread;

    std::mutex m_oneTimeTasksMutex;
    std::condition_variable m_oneTimeTasksCV;
    std::queue<std::function<void()>> m_oneTimeTasks;
};

template <typename Task, typename... Args>
void Thread::addTask(Task task, Args&&... args) {
    auto boundTask = std::bind(task, std::forward<Args>(args)...);

    auto noReturnTask = [boundTask] {
        boundTask();
    };

    {
        std::unique_lock<std::mutex> lock(m_oneTimeTasksMutex);
        m_oneTimeTasks.push(noReturnTask);
        m_oneTimeTasksCV.notify_all();
    }
}

}  // namespace srmt
}  // namespace net

#endif  // REACTOR_SINGLE_REACTOR_MULTI_THREADS_THREAD_H_
