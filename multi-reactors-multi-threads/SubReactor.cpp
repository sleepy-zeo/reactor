#include "SubReactor.h"

#include "Acceptor.h"
#include <Logger.h>

static const std::string TAG{"SubReactor"};

net::mrmt::SubReactor::SubReactor(
    std::shared_ptr<Acceptor> acceptor,
    std::shared_ptr<EventLoop> loop,
    std::string name,
    int client_socket_fd) {
    this->m_acceptor = acceptor;
    this->m_eventLoop = loop;
    this->m_subReactorName = std::move(name);
    this->m_connection = std::make_shared<Connection>(loop.get(), name, client_socket_fd);

    m_connection->setMessageCallback(
        std::bind(&SubReactor::onMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
    m_connection->setWriteCallback(std::bind(&SubReactor::onMessageSent, this, std::placeholders::_1));
    m_connection->setCloseCallback(std::bind(&SubReactor::onConnectionClosed, this, std::placeholders::_1));
}

net::mrmt::SubReactor::~SubReactor() {
    close();
}

void net::mrmt::SubReactor::onMessageReceived(const std::weak_ptr<Connection>& c, const std::string& msg) {
    ACSDK_DEBUG(LX(__func__).d("threadId", std::this_thread::get_id()).d("msg", msg));

    if (msg == "hello")
        m_connection->send("hello, this is mrmt");
    if (msg == "close")
        m_connection->forceClose();
}

void net::mrmt::SubReactor::onMessageSent(const std::weak_ptr<Connection>& c) {
    ACSDK_DEBUG(LX(__func__));
}

void net::mrmt::SubReactor::onConnectionClosed(const std::weak_ptr<Connection>& c) {
    ACSDK_DEBUG(LX(__func__));
    if (m_acceptor) {
        m_acceptor->removeSubReactor(this->m_connection->getSocketFd());
    }
}

void net::mrmt::SubReactor::close() {
    m_connection.reset();
    m_connection = nullptr;
}
