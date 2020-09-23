#ifndef REACTOR_SINGLE_REACTOR_MULTI_THREADS_POLLER_H_
#define REACTOR_SINGLE_REACTOR_MULTI_THREADS_POLLER_H_

#include <vector>

namespace net {
namespace srmt {

class Poller {

public:
    Poller() = default;
    virtual ~Poller() = default;

    virtual void poll(int timeoutMs, std::vector<Channel*>* activeChannels) = 0;
    virtual bool updateChannel(Channel* channel) = 0;
    virtual bool removeChannel(Channel* channel) = 0;
};

}  // namespace srmt
}  // namespace net

#endif  // REACTOR_SINGLE_REACTOR_MULTI_THREADS_POLLER_H_
