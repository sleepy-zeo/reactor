#ifndef REACTOR_SINGLE_REACTOR_SINGLE_THREAD_CHANNEL_H_
#define REACTOR_SINGLE_REACTOR_SINGLE_THREAD_CHANNEL_H_

#include <functional>

namespace net {
namespace srst {

class EventLoop;

class Channel {

public:
    enum class State : uint8_t {
        ESTABLISHED = 0x1F,
        ADDED,
    };

public:
    Channel(EventLoop* loop, int m_fd);
    ~Channel();

    void handleEvent();

    int getFd() const;
    int getEventsConfigured() const;
    void setEventsConfigured(int m_events_configured);
    int getEventsReceived() const;
    void setEventsReceived(int m_events_received);

    void setReadCallback(const std::function<void()>& m_read_callback);
    void setWriteCallback(const std::function<void()>& m_write_callback);
    void setErrorCallback(const std::function<void()>& m_error_callback);
    void setCloseCallback(const std::function<void()>& m_close_callback);

    bool enableReading();
    bool disableReading();
    bool enableWriting();
    bool disableWriting();
    bool disableAll();
    bool remove();

    State getState() const;
    void setState(State m_state);

private:
    EventLoop* m_eventLoop;

    const int m_fd;
    int m_events_configured;
    int m_events_received;

    State m_state;

    std::function<void()> m_readCallback;
    std::function<void()> m_writeCallback;
    std::function<void()> m_errorCallback;
    std::function<void()> m_closeCallback;
};
}  // namespace srst
}  // namespace net

#endif  // REACTOR_SINGLE_REACTOR_SINGLE_THREAD_CHANNEL_H_
