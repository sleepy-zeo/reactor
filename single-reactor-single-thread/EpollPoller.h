#ifndef REACTOR_SINGLE_REACTOR_SINGLE_THREAD_EPOLLPOLLER_H_
#define REACTOR_SINGLE_REACTOR_SINGLE_THREAD_EPOLLPOLLER_H_

#include "Channel.h"
#include "Poller.h"
#include <map>
#include <sys/epoll.h>
#include <unordered_map>
#include <vector>

namespace net {
namespace srst {

class EpollPoller : public Poller {

public:
    EpollPoller();
    ~EpollPoller() override;

    void poll(int timeoutMs, std::vector<Channel*>* activeChannels) override;
    bool update(int operation, Channel* channel);

    bool updateChannel(Channel* channel) override;
    bool removeChannel(Channel* channel) override;

private:
    int m_epollFd;
    std::vector<epoll_event> m_eventList;
    std::unordered_map<int, Channel*> m_channelMap;
};

}  // namespace srst
}  // namespace net

#endif  // REACTOR_SINGLE_REACTOR_SINGLE_THREAD_EPOLLPOLLER_H_
