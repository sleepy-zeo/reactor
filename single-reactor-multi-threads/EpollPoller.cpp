#include "EpollPoller.h"

#include <Logger.h>
#include <cstring>
#include <string>
#include <zconf.h>

static const std::string TAG{"EpollPoller"};
static const int INIT_EVENT_LIST_SIZE = 128;

net::srmt::EpollPoller::EpollPoller()
        : m_eventList(INIT_EVENT_LIST_SIZE) {
    m_epollFd = ::epoll_create(128);
    if (m_epollFd < 0) {
        ACSDK_ERROR(LX(__func__).d("m_epollFd", m_epollFd));
    }
}

net::srmt::EpollPoller::~EpollPoller() {
    ::close(m_epollFd);
}

void net::srmt::EpollPoller::poll(int timeoutMs, std::vector<Channel*>* activeChannels) {
    int numEvents = ::epoll_wait(m_epollFd, &*m_eventList.begin(), m_eventList.size(), timeoutMs);
    int savedErrno = errno;
    if (numEvents > 0) {
        ACSDK_DEBUG9(LX(__func__).d("numEvents", numEvents));
        for (int i = 0; i < numEvents; ++i) {
            auto* channel = reinterpret_cast<Channel*>(m_eventList[i].data.ptr);
            if (m_channelMap.find(channel->getFd()) == m_channelMap.end() ||
                m_channelMap.find(channel->getFd())->second != channel) {
                continue;
            }
            channel->setEventsReceived(m_eventList[i].events);
            if (activeChannels != nullptr) {
                activeChannels->push_back(channel);
            }
        }
        if (numEvents == m_eventList.size()) {
            m_eventList.resize(m_eventList.size() * 2);
        }
    } else if (numEvents == 0) {
        // ACSDK_DEBUG(LX(__func__).m("nothing happened"));
    } else {
        if (savedErrno != EINTR) {
            ACSDK_ERROR(LX(__func__).d("errno", errno));
            errno = savedErrno;
        }
    }
}

bool net::srmt::EpollPoller::update(int operation, net::srmt::Channel* channel) {
    epoll_event event{};
    memset(&event, 0, sizeof(event));
    event.events = channel->getEventsConfigured();
    event.data.ptr = channel;
    int fd = channel->getFd();
    int epoll_result = ::epoll_ctl(m_epollFd, operation, fd, &event);
    if (epoll_result < 0) {
        ACSDK_ERROR(LX(__func__)
                        .d("operation", operation)
                        .d("fd", fd)
                        .d("epollFd", m_epollFd)
                        .d("errno", errno)
                        .d("errorInfo", strerror(errno)));
        return false;
    }
    return true;
}

bool net::srmt::EpollPoller::updateChannel(net::srmt::Channel* channel) {
    if (channel->getState() == Channel::State::ESTABLISHED) {
        if (m_channelMap.find(channel->getFd()) != m_channelMap.end()) {
            ACSDK_ERROR(LX(__func__).m("m_channelMap contains the established channel"));
            return false;
        }
        bool result = update(EPOLL_CTL_ADD, channel);
        if (result) {
            channel->setState(Channel::State::ADDED);
            m_channelMap.emplace(channel->getFd(), channel);
            return true;
        }
        ACSDK_ERROR(LX(__func__).m("EPOLL_CTL_ADD failed"));
        return false;

    } else {
        if (m_channelMap.find(channel->getFd()) == m_channelMap.end() || m_channelMap[channel->getFd()] != channel) {
            ACSDK_ERROR(LX(__func__).m("channel not valid"));
            return false;
        }
        return update(EPOLL_CTL_MOD, channel);
    }
}

bool net::srmt::EpollPoller::removeChannel(net::srmt::Channel* channel) {
    ACSDK_INFO(LX(__func__));
    if (channel->getState() == Channel::State::ESTABLISHED) {
        ACSDK_ERROR(LX(__func__).m("channel's state is Channel::State::ESTABLISHED"));
        return false;
    } else {
        if (m_channelMap.find(channel->getFd()) == m_channelMap.end() || m_channelMap[channel->getFd()] != channel) {
            ACSDK_ERROR(LX(__func__).m("channel not valid"));
            return false;
        }
        bool result = update(EPOLL_CTL_DEL, channel);
        if (result) {
            channel->setState(Channel::State::ESTABLISHED);
            m_channelMap.erase(channel->getFd());
            return true;
        }
    }
    ACSDK_ERROR(LX(__func__).m("EPOLL_CTL_DEL failed"));
    return false;
}
