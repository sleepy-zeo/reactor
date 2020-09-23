#include "Connection.h"

#include <Logger.h>
#include <cstring>
#include <memory>
#include <string>
#include <unistd.h>

static const std::string TAG{"Connection"};

net::srst::Connection::Connection(net::srst::EventLoop* loop, std::string name, int socketFd)
        : m_bufferIn{}
        , m_bufferOut{} {
    this->m_name = std::move(name);
    this->m_channel = std::make_unique<Channel>(loop, socketFd);
    this->m_channel->setReadCallback(std::bind(&Connection::handleRead, this));
    this->m_channel->setWriteCallback(std::bind(&Connection::handleWrite, this));
    this->m_channel->setErrorCallback(std::bind(&Connection::handleError, this));
    this->m_channel->setCloseCallback(std::bind(&Connection::handleClose, this));
    this->m_channel->enableReading();
}

net::srst::Connection::~Connection() {
    ACSDK_DEBUG(LX(__func__));
    m_channel.reset();
}

void net::srst::Connection::setMessageCallback(
    const std::function<void(const std::weak_ptr<Connection>, std::string)>& m_message_callback) {
    m_messageCallback = m_message_callback;
}

void net::srst::Connection::setWriteCallback(
    const std::function<void(const std::weak_ptr<Connection>)>& m_write_callback) {
    m_writeCallback = m_write_callback;
}

void net::srst::Connection::setCloseCallback(
    const std::function<void(const std::weak_ptr<Connection>)>& m_close_callback) {
    m_closeCallback = m_close_callback;
}

void net::srst::Connection::send(const std::string& message) {
    int len = message.length();
    char* data = new char[message.size()];
    memcpy(data, message.c_str(), message.length());

    m_bufferOut.write(data, len);
    m_channel->enableWriting();
}

void net::srst::Connection::handleRead() {
    int savedErrno = 0;
    size_t n = m_bufferIn.readFd(m_channel->getFd(), &savedErrno);
    if (n > 0) {
        int readableSize = m_bufferIn.readableBytes();
        char* msg = m_bufferIn.read(readableSize);
        if (m_messageCallback) {
            m_messageCallback(shared_from_this(), std::string(msg, readableSize));
        }
    } else {
        handleClose();
    }
}

void net::srst::Connection::handleWrite() {
    size_t n = write(this->m_channel->getFd(), &*m_bufferOut.getBuffer()->begin(), m_bufferOut.readableBytes());
    if (n > 0) {
        m_bufferOut.retrieve(n);
        if (m_bufferOut.readableBytes() == 0) {
            m_channel->disableWriting();
            if (m_writeCallback) {
                m_writeCallback(shared_from_this());
            }
        }
    }
}

void net::srst::Connection::handleError() {
    ACSDK_DEBUG0(LX(__func__).d("name", m_name).d("error", errno));
    handleClose();
}

void net::srst::Connection::handleClose() {
    ACSDK_INFO(LX(__func__));
    m_channel->disableAll();
    m_channel->remove();

    if (m_closeCallback) {
        m_closeCallback(shared_from_this());
    }
}

void net::srst::Connection::forceClose() {
    ACSDK_DEBUG(LX(__func__));
    handleClose();
}

int net::srst::Connection::getSocketFd() {
    return m_channel->getFd();
}
