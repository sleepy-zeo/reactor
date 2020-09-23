#include "Utilities.h"

#include <Logger.h>
#include <string>

static const std::string TAG{"Utilities"};

void net::srmt::setReuseAddr(int socket_fd, bool on) {
    int optval = on ? 1 : 0;

    ///
    /// @link https://blog.csdn.net/yaokai_assultmaster/article/details/68951150
    ///
    /// 作用域: 原生BSD系统
    ///
    /// SO_REUSEADDR:
    /// 1. 处理通配符ip的优势: 如果不用SO_REUSEADDR的话，如果我们将socketA绑定到0.0.0.0:21，那么任何将本机其他socket绑定
    /// 到端口21的举动(如绑定到192.168.1.1:21)都会导致EADDRINUSE错误，这是因为0.0.0.0是一个通配符IP地址，意味着任意一个
    /// IP地址，所以任何其他本机上的IP地址都被系统认为已被占用，通过对后面的socket设置SO_REUSEADDR就不会再导致EADDRINUSE错误了
    ///
    /// 2. 处理TIME_WAIT的优势:
    /// 系统在真正关闭socket前都会有一个TIME_WAIT的阶段，处于TIME_WAIT阶段的socket任然被认为是绑定在
    /// 原来那个地址和端口上的，直到该socket被完全关闭之前。在这个时间内任何企图将一个新socket绑定到该(地址:端口)对的操作都无法成功
    /// 如果我们在新的socket上设置了SO_REUSEADDR选项，如果此时有另一个socket绑定在当前的地址端口对且处于TIME_WAIT阶段，那么这个已
    /// 存在的绑定关系将会被忽略，当然如果旧的socket还在运行状态，而不是TIME_WAIT阶段，那么新的肯定会绑定失败的
    ///
    /// 以上内容只对新的socket设置了SO_REUSEADDR就可以了
    ///
    ///
    /// *作用域: Linux
    /// 简单来说只有第二个在linux上有效，第一个无效(这里的无效针对server端，client则没有限制)
    ///
    int ret = ::setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
    if (ret < 0 && on) {
        ACSDK_ERROR(LX(__func__).m("setReuseAddr failed").d("errno", errno));
    }
}

void net::srmt::setReusePort(int socket_fd, bool on) {
    int optval = on ? 1 : 0;

    ///
    /// @link https://blog.csdn.net/yaokai_assultmaster/article/details/68951150
    ///
    /// 作用域: 原生BSD系统
    ///
    /// SO_REUSEPORT:
    /// 处理TIME_WAIT:
    /// 这里需要绑定相同(地址:端口)对的所有socket都必须设置SO_REUSEPORT，否则如果当前socket还处于TIME_WAIT阶段的话后面的socket都会绑定失败
    ///
    /// *作用域: Linux
    /// 只要所有的socket(无其他限制，也就是tcp or upd，server or
    /// client这些不限制)都设置了该选项就可以绑定在完全相同的(地址:端口)对下
    /// 这么做的好处是一个简单的服务器进程的几个不同实例可以方便地使用SO_REUSEPORT来实现一个简单的负载均衡
    ///
    int ret = ::setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
    if (ret < 0 && on) {
        ACSDK_ERROR(LX(__func__).m("setReusePort failed").d("errno", errno));
    }
}

void net::srmt::setNoBlock(int socket_fd) {
    // non-block
    int flags = ::fcntl(socket_fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    ::fcntl(socket_fd, F_SETFL, flags);
}

void net::srmt::setCloseOnExec(int socket_fd) {
    // close-on-exec
    int flags = ::fcntl(socket_fd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ::fcntl(socket_fd, F_SETFD, flags);
}