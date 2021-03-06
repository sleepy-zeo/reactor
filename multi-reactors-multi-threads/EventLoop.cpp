#include "EventLoop.h"

#include "EpollPoller.h"
#include <Logger.h>
#include <thread>

const std::string TAG{"EventLoop"};

const int kPollTimeMs = 1000;  // ms

net::mrmt::EventLoop::EventLoop()
        : m_quit{false}
        , m_poller(new net::mrmt::EpollPoller()) {
}

void net::mrmt::EventLoop::loop() {
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

void net::mrmt::EventLoop::quit() {
    m_quit = true;
}

bool net::mrmt::EventLoop::updateChannel(net::mrmt::Channel* channel) {
    return m_poller->updateChannel(channel);
}

bool net::mrmt::EventLoop::removeChannel(net::mrmt::Channel* channel) {
    return m_poller->removeChannel(channel);
}

void net::mrmt::EventLoop::addOneTimeTask(const std::function<void()>& oneTimeTask) {
    m_oneTimeTasks.push_back(oneTimeTask);
}
