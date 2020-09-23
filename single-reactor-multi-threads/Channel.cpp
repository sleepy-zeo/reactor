#include "Channel.h"

#include "EventLoop.h"
#include "Utilities.h"
#include <Logger.h>
#include <string>
#include <sys/poll.h>
#include <unistd.h>

const std::string TAG{"Channel"};

net::srmt::Channel::Channel(EventLoop* loop, int m_fd)
        : m_eventLoop(loop)
        , m_fd(m_fd)
        , m_events_configured(0)
        , m_state(State::ESTABLISHED) {

    net::srmt::setReusePort(m_fd, true);
    net::srmt::setReuseAddr(m_fd, true);
    net::srmt::setNoBlock(m_fd);
    net::srmt::setCloseOnExec(m_fd);
}

net::srmt::Channel::~Channel() {
    ACSDK_DEBUG(LX(__func__));
    ::close(m_fd);
}

int net::srmt::Channel::getFd() const {
    return m_fd;
}

int net::srmt::Channel::getEventsConfigured() const {
    return m_events_configured;
}

void net::srmt::Channel::setEventsConfigured(int m_events_configured) {
    Channel::m_events_configured = m_events_configured;
}

int net::srmt::Channel::getEventsReceived() const {
    return m_events_received;
}

void net::srmt::Channel::setEventsReceived(int m_events_received) {
    Channel::m_events_received = m_events_received;
}

void net::srmt::Channel::handleEvent() {
    if ((m_events_received & POLLHUP) && !(m_events_received & POLLIN)) {
        if (m_closeCallback)
            m_closeCallback();
    }

    if (m_events_received & POLLNVAL) {
        ACSDK_WARN(LX(__func__).m("handle event POLLNVAL"));
    }

    if (m_events_received & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (m_readCallback) {
            m_readCallback();
        }
    }

    if (m_events_received & POLLOUT) {
        if (m_writeCallback)
            m_writeCallback();
    }
}

void net::srmt::Channel::setReadCallback(const std::function<void()>& m_read_callback) {
    m_readCallback = m_read_callback;
}

void net::srmt::Channel::setWriteCallback(const std::function<void()>& m_write_callback) {
    m_writeCallback = m_write_callback;
}

void net::srmt::Channel::setErrorCallback(const std::function<void()>& m_error_callback) {
    m_errorCallback = m_error_callback;
}

void net::srmt::Channel::setCloseCallback(const std::function<void()>& m_close_callback) {
    m_closeCallback = m_close_callback;
}

bool net::srmt::Channel::enableReading() {
    m_events_configured |= POLLIN | POLLPRI;
    return m_eventLoop->updateChannel(this);
}

bool net::srmt::Channel::disableReading() {
    m_events_configured &= ~(POLLIN | POLLPRI);
    return m_eventLoop->updateChannel(this);
}

bool net::srmt::Channel::enableWriting() {
    m_events_configured |= POLLOUT;
    return m_eventLoop->updateChannel(this);
}

bool net::srmt::Channel::disableWriting() {
    m_events_configured &= ~POLLOUT;
    return m_eventLoop->updateChannel(this);
}

bool net::srmt::Channel::disableAll() {
    m_events_configured = 0;
    return m_eventLoop->updateChannel(this);
}

net::srmt::Channel::State net::srmt::Channel::getState() const {
    return m_state;
}

void net::srmt::Channel::setState(net::srmt::Channel::State m_state) {
    Channel::m_state = m_state;
}

bool net::srmt::Channel::remove() {
    m_eventLoop->removeChannel(this);
}
