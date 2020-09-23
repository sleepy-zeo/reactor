#ifndef REACTOR_SINGLE_REACTOR_SINGLE_THREAD_CONNECTION_H_
#define REACTOR_SINGLE_REACTOR_SINGLE_THREAD_CONNECTION_H_

#include "EventLoop.h"
#include <Buffer.h>

namespace net {
namespace srst {

class Connection : public std::enable_shared_from_this<Connection> {

public:
    Connection(EventLoop* loop, std::string name, int socketFd);
    ~Connection();

    void send(const std::string& message);

    void setMessageCallback(
        const std::function<void(const std::weak_ptr<Connection>, std::string)>& m_message_callback);
    void setWriteCallback(const std::function<void(const std::weak_ptr<Connection>)>& m_write_callback);
    void setCloseCallback(const std::function<void(const std::weak_ptr<Connection>)>& m_close_callback);

    int getSocketFd();

    void forceClose();

private:
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    std::string m_name;
    std::unique_ptr<Channel> m_channel;

    // read
    Buffer m_bufferIn;
    // write
    Buffer m_bufferOut;

    std::function<void(const std::weak_ptr<Connection>, std::string msg)> m_messageCallback;
    std::function<void(const std::weak_ptr<Connection>)> m_writeCallback;
    std::function<void(const std::weak_ptr<Connection>)> m_closeCallback;
};

}  // namespace srst
}  // namespace net

#endif  // REACTOR_SINGLE_REACTOR_SINGLE_THREAD_CONNECTION_H_
