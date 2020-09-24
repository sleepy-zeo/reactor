#ifndef REACTOR_MULTI_REACTORS_MULTI_THREADS_SUBREACTOR_H_
#define REACTOR_MULTI_REACTORS_MULTI_THREADS_SUBREACTOR_H_

#include "Connection.h"
#include "EventLoop.h"
#include <string>

namespace net {
namespace mrmt {

class Acceptor;
class SubReactor {

public:
    SubReactor(
        std::shared_ptr<Acceptor> acceptor,
        std::shared_ptr<EventLoop> loop,
        std::string name,
        int client_socket_fd);

    ~SubReactor();

private:
    void onMessageReceived(const std::weak_ptr<Connection>& c, const std::string& msg);
    void onMessageSent(const std::weak_ptr<Connection>& c);
    void onConnectionClosed(const std::weak_ptr<Connection>& c);

    void close();

    std::shared_ptr<Connection> m_connection;
    std::shared_ptr<EventLoop> m_eventLoop;

    std::string m_subReactorName;
    std::shared_ptr<Acceptor> m_acceptor;
};

}  // namespace mrmt
}  // namespace net

#endif  // REACTOR_MULTI_REACTORS_MULTI_THREADS_SUBREACTOR_H_
