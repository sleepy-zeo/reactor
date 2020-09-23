#ifndef REACTOR_SINGLE_REACTOR_MULTI_THREADS_ACCEPTOR_H_
#define REACTOR_SINGLE_REACTOR_MULTI_THREADS_ACCEPTOR_H_

#include "EventLoop.h"
#include "Thread.h"
#include <map>
#include <unordered_map>

namespace net {
namespace srmt {

class Connection;
class Acceptor {

public:
    Acceptor(EventLoop* loop, const std::string& ip, uint16_t port);
    ~Acceptor() = default;

    void listen();
    void handleRead();

private:
    int m_acceptSocket;
    EventLoop* m_eventLoop;
    Channel* m_acceptChannel;
    std::unordered_map<int, std::shared_ptr<Connection>> m_connections;
    Thread taskProcessThread;
};

}  // namespace srmt
}  // namespace net

#endif  // REACTOR_SINGLE_REACTOR_MULTI_THREADS_ACCEPTOR_H_
