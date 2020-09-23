#include <Acceptor.h>
#include <EventLoop.h>
#include <csignal>
#include <execinfo.h>
#include <iostream>
#include <zconf.h>

int main() {
    auto* loop = new net::srst::EventLoop();
    auto* acceptor = new net::srst::Acceptor(loop, "127.0.0.1", 23345);
    acceptor->listen();
    loop->loop();

    return 0;
}
