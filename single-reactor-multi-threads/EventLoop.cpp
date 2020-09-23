#include "EventLoop.h"

#include "EpollPoller.h"
#include <Logger.h>
#include <thread>

const std::string TAG{"EventLoop"};

const int kPollTimeMs = 1000;  // ms

net::srmt::EventLoop::EventLoop()
        : m_quit{false}
        , m_poller(new net::srmt::EpollPoller()) {
}

void net::srmt::EventLoop::loop() {
    ACSDK_DEBUG(LX("looping..."));
    m_quit = false;

    while (!m_quit) {
        m_activeChannels.clear();
        m_poller->poll(kPollTimeMs, &m_activeChannels);
        if (m_activeChannels.empty()) {
            // ACSDK_DEBUG9(LX(__func__).m("no events received, timeout..."));
        } else {
            // ACSDK_DEBUG(LX(__func__).m("receiving events..."));
        }
        for (auto iter = m_activeChannels.begin(); iter != m_activeChannels.end(); ++iter) {
            m_currentChannel = *iter;
            m_currentChannel->handleEvent();
        }
        m_currentChannel = nullptr;

        if (!m_oneTimeTasks.empty()) {
            for (auto iter = m_oneTimeTasks.begin(); iter != m_oneTimeTasks.end(); ++iter) {
                iter->operator()();
            }
            m_oneTimeTasks.clear();
        }
    }
    ACSDK_DEBUG0(LX(__func__).m("stop Looping...").d("eventLoop", this).d("threadId", std::this_thread::get_id()));
}

bool net::srmt::EventLoop::updateChannel(net::srmt::Channel* channel) {
    return m_poller->updateChannel(channel);
}

bool net::srmt::EventLoop::removeChannel(net::srmt::Channel* channel) {
    return m_poller->removeChannel(channel);
}

void net::srmt::EventLoop::addOneTimeTask(const std::function<void()>& oneTimeTask) {
    m_oneTimeTasks.push_back(oneTimeTask);
}
