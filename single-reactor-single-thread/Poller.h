#ifndef REACTOR_SINGLE_REACTOR_SINGLE_THREAD_POLLER_H_
#define REACTOR_SINGLE_REACTOR_SINGLE_THREAD_POLLER_H_

#include <vector>

namespace net {
namespace srst {

class Poller {

public:
    Poller() = default;
    virtual ~Poller() = default;

    virtual void poll(int timeoutMs, std::vector<Channel*>* activeChannels) = 0;
    virtual bool updateChannel(Channel* channel) = 0;
    virtual bool removeChannel(Channel* channel) = 0;
};

}  // namespace srst
}  // namespace net

#endif  // REACTOR_SINGLE_REACTOR_SINGLE_THREAD_POLLER_H_
