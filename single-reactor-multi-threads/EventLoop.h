#ifndef REACTOR_SINGLE_REACTOR_MULTI_THREADS_EVENTLOOP_H_
#define REACTOR_SINGLE_REACTOR_MULTI_THREADS_EVENTLOOP_H_

#include "Channel.h"
#include "Poller.h"
#include <atomic>
#include <deque>
#include <memory>
#include <vector>

namespace net {
namespace srmt {

class EventLoop {

public:
    EventLoop();
    ~EventLoop() = default;

    void loop();

    bool updateChannel(Channel* channel);
    bool removeChannel(Channel* channel);

    void addOneTimeTask(const std::function<void()>& oneTimeTask);

private:
    std::atomic<bool> m_quit;
    std::unique_ptr<Poller> m_poller;
    std::vector<Channel*> m_activeChannels;
    Channel* m_currentChannel;

    std::deque<std::function<void()>> m_oneTimeTasks;
};

}  // namespace srmt
}  // namespace net

#endif  // REACTOR_SINGLE_REACTOR_MULTI_THREADS_EVENTLOOP_H_
