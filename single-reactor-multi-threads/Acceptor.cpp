#include "Acceptor.h"

#include "Connection.h"
#include "Utilities.h"
#include <Logger.h>
#include <arpa/inet.h>
#include <cstring>
#include <deque>
#include <future>
#include <memory>

const std::string TAG{"Acceptor"};

net::srmt::Acceptor::Acceptor(net::srmt::EventLoop* loop, const std::string& ip, uint16_t port) {
    this->m_eventLoop = loop;

    int socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    net::srmt::setReuseAddr(socket, true);
    net::srmt::setReusePort(socket, true);
    struct sockaddr_in server_config {};
    memset(&server_config, 0, sizeof(server_config));
    server_config.sin_family = AF_INET;
    server_config.sin_addr.s_addr = inet_addr(ip.c_str());
    server_config.sin_port = htons(port);
    bind(socket, (struct sockaddr*)&server_config, sizeof(server_config));

    this->m_acceptSocket = socket;
    this->m_acceptChannel = new Channel(m_eventLoop, this->m_acceptSocket);
    this->m_acceptChannel->setReadCallback(std::bind(&net::srmt::Acceptor::handleRead, this));
}

void net::srmt::Acceptor::listen() {
    ACSDK_DEBUG(LX(__func__));
    int ret = ::listen(this->m_acceptSocket, SOMAXCONN);
    if (ret < 0) {
        throw std::runtime_error("listenOrDie failed");
    }
    this->m_acceptChannel->enableReading();
}

class A {
public:
    A() {
        ACSDK_DEBUG(LX("AAA"));
        year = 2020;
    }
    ~A() {
        ACSDK_DEBUG(LX("~AAA"));
        year = 0;
    }
    const void print() const {
        ACSDK_DEBUG(LX("print").d("year", year));
    }
    int year;
};

void net::srmt::Acceptor::handleRead() {
    sockaddr_in peer{};
    socklen_t peer_len = sizeof(peer);

    int client_socket_fd = accept(m_acceptSocket, (sockaddr*)&peer, &peer_len);
    if (client_socket_fd > 0) {
        ACSDK_INFO(LX("Acceptor").m("establish a new connection").d("client_socket", client_socket_fd));
        auto connection = std::make_shared<Connection>(m_eventLoop, "", client_socket_fd);
        m_connections.emplace(client_socket_fd, connection);

        connection->setMessageCallback([this, client_socket_fd](const std::weak_ptr<Connection>& c, const std::string& msg) {
            std::shared_ptr<Connection> conn = c.lock();
            if (conn) {

                auto task = [](const std::weak_ptr<Connection>& weakConnection, const std::string& data) {
                    auto connection = weakConnection.lock();
                    if (connection) {
                        if (data == "hello") {
                            connection->send("hi");
                        }
                        if (data == "close") {
                            connection->forceClose();
                        }
                    }
                };
                auto& connection = m_connections[client_socket_fd];
                taskProcessThread.addTask(task, connection, msg);
            }
        });
        connection->setWriteCallback([](const std::weak_ptr<Connection>& c) {
            ACSDK_INFO(LX("Acceptor").m("onMessageSend"));
        });
        connection->setCloseCallback([this](const std::weak_ptr<Connection>& c) {
            std::shared_ptr<Connection> conn = c.lock();
            if (conn) {
                ACSDK_INFO(LX("Acceptor").m("destruct a new connection").d("client_socket", conn->getSocketFd()));
                m_eventLoop->addOneTimeTask([this, conn_weak = std::weak_ptr<Connection>(conn)] {
                    auto conn_shared = conn_weak.lock();
                    if (conn_shared) {
                        m_connections.erase(conn_shared->getSocketFd());
                    }
                });
            }
        });
    }
}