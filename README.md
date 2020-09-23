# reactor

> 单Reactor单线程

```text
Reactor通过poll监控所有的事件，收到事件后通过dispatch分发
        - 如果是建立连接的事件，调用Acceptor处理，并创建Handler处理该连接的后续事件
        - 如果是其他事件，Reactor找到对应的Handler来处理该事件

Handler完成 read -> 处理 -> send 流程
```


> 单Reactor多线程

```text
Reactor通过poll监控所有的事件，收到事件后通过dispatch分发
        - 如果是建立连接的事件，调用Acceptor处理，并创建Handler处理该连接的后续事件
        - 如果是其他事件，Reactor找到对应的Handler来处理该事件

Handler读取完数据 -> 交给子线程的Processor进行处理 -> 处理完结果发回给Handler -> Handler将结果send给client
```


> 多Reactor多线程

```text
mainReactor(主线程或者子线程)通过poll监控所有的建立连接事件，收到的建立连接事件发送给Acceptor，由Acceptor将新的连接分配给某个子线程
        - subReactor将Acceptor分配的连接加入连接队列进行监听，并创建一个 Handler用于处理连接的各种事件
        - subReactor监听该连接的所有的新的事件，并调用自身的Handler来进行响应

Handler完成 read -> 处理 -> send 流程
```



