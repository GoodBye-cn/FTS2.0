#include "Acceptor.h"
#include "Handler.h"

Acceptor::Acceptor(/* args */) {}

Acceptor::~Acceptor() {}

void Acceptor::accept_conn(struct evconnlistener* listener,
    evutil_socket_t fd, struct sockaddr* address, int socklen,
    void* ctx) {
    /* 此时已经接收到了文件描述符，调用Handler处理读写事件 */
    event_base* base = evconnlistener_get_base(listener);
    Handler* handler = new Handler();
    handler->set_base(base);
    handler->set_fd(fd);
    handler->init();
}