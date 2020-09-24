#ifndef REACTOR_MULTI_REACTORS_MULTI_THREADS_ACCEPTOR_H_
#define REACTOR_MULTI_REACTORS_MULTI_THREADS_ACCEPTOR_H_

#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "SubReactor.h"
#include <map>
#include <unordered_map>

namespace net {
namespace mrmt {

class Connection;

class Acceptor : public std::enable_shared_from_this<Acceptor> {

public:
    Acceptor(EventLoop* loop, const std::string& ip, uint16_t port);
    ~Acceptor() = default;

    void listen();
    void handleRead();

    void removeSubReactor(int socketFd);

private:
    int m_acceptSocket;
    EventLoop* m_eventLoop;
    Channel* m_acceptChannel;
    std::unordered_map<int, std::shared_ptr<SubReactor>> m_subReactors;
    std::shared_ptr<EventLoopThreadPool> m_eventLoopThreadPool;
};

}  // namespace mrmt
}  // namespace net

#endif  // REACTOR_MULTI_REACTORS_MULTI_THREADS_ACCEPTOR_H_
