# Thread

> 提交task立即执行

```c++
auto connection = m_connections[client_socket_fd];
std::string msg = "hello";

std::thread processTaskThread([connection_weak = std::weak_ptr<Connection>(connection), msg] {
    auto connection = connection_weak.lock();
    if (connection) {
        if (msg == "hello") {
            // connection->send("xxxx");
        }
    }
});
```

> 提交task延迟执行

```c++
// Sample
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

// Usage
auto task = [](const std::weak_ptr<Connection>& weakConnection, const std::string& data) {
    auto connection = weakConnection.lock();
    if (connection) {
        if (data == "hello") {
            connection->send("hi");
        }
        if (data == "close") {
            connection->forceClose();
        }
    }
};
auto& connection = m_connections[client_socket_fd];
taskProcessThread.addTask(task, connection, msg);
```