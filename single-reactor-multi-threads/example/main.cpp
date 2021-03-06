#include <Acceptor.h>
#include <EventLoop.h>
#include <iostream>

int main() {
    auto* loop = new net::srmt::EventLoop();
    auto acceptor = std::make_shared<net::srmt::Acceptor>(loop, "127.0.0.1", 23345);
    acceptor->listen();
    loop->loop();

    return 0;
}
