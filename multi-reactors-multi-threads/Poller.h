#ifndef REACTOR_MULTI_REACTORS_MULTI_THREADS_POLLER_H_
#define REACTOR_MULTI_REACTORS_MULTI_THREADS_POLLER_H_

#include <vector>

namespace net {
namespace mrmt {

class Poller {

public:
    Poller() = default;
    virtual ~Poller() = default;

    virtual void poll(int timeoutMs, std::vector<Channel*>* activeChannels) = 0;
    virtual bool updateChannel(Channel* channel) = 0;
    virtual bool removeChannel(Channel* channel) = 0;
};

}  // namespace mrmt
}  // namespace net

#endif  // REACTOR_MULTI_REACTORS_MULTI_THREADS_POLLER_H_
