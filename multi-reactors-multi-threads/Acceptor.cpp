#include "Acceptor.h"

#include "Connection.h"
#include "SubReactor.h"
#include "Utilities.h"
#include <Logger.h>
#include <arpa/inet.h>
#include <cstring>
#include <deque>
#include <future>
#include <memory>

const std::string TAG{"Acceptor"};

net::mrmt::Acceptor::Acceptor(net::mrmt::EventLoop* loop, const std::string& ip, uint16_t port) {
    this->m_eventLoop = loop;

    int socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    net::mrmt::setReuseAddr(socket, true);
    net::mrmt::setReusePort(socket, true);
    struct sockaddr_in server_config {};
    memset(&server_config, 0, sizeof(server_config));
    server_config.sin_family = AF_INET;
    server_config.sin_addr.s_addr = inet_addr(ip.c_str());
    server_config.sin_port = htons(port);
    bind(socket, (struct sockaddr*)&server_config, sizeof(server_config));

    this->m_acceptSocket = socket;
    this->m_acceptChannel = new Channel(m_eventLoop, this->m_acceptSocket);
    this->m_acceptChannel->setReadCallback(std::bind(&net::mrmt::Acceptor::handleRead, this));

    this->m_eventLoopThreadPool = std::make_shared<EventLoopThreadPool>(5);
    this->m_eventLoopThreadPool->start();
}

void net::mrmt::Acceptor::listen() {
    ACSDK_DEBUG(LX(__func__));
    int ret = ::listen(this->m_acceptSocket, SOMAXCONN);
    if (ret < 0) {
        throw std::runtime_error("listenOrDie failed");
    }
    this->m_acceptChannel->enableReading();
}

void net::mrmt::Acceptor::handleRead() {
    sockaddr_in peer{};
    socklen_t peer_len = sizeof(peer);

    int client_socket_fd = accept(m_acceptSocket, (sockaddr*)&peer, &peer_len);
    if (client_socket_fd > 0) {
        ACSDK_INFO(LX("Acceptor").m("establish a new connection").d("client_socket", client_socket_fd));

        std::shared_ptr<SubReactor> subReactor = std::make_shared<SubReactor>(
            shared_from_this(), this->m_eventLoopThreadPool->getNextLoop(), "", client_socket_fd);

        m_subReactors.emplace(client_socket_fd, subReactor);
    }
}

void net::mrmt::Acceptor::removeSubReactor(int socketFd) {
    m_subReactors.erase(socketFd);
}
