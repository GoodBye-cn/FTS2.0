#include "Acceptor.h"
#include "Handler.h"
#include "Reactor.h"

Acceptor::Acceptor() {
    count = 0;
}

Acceptor::~Acceptor() {}

void Acceptor::add_client_address(Handler* handler, sockaddr_in* addr) {
    client_addres[handler] = *addr;
}

void Acceptor::remove_client_address(Handler* handler) {
    sockaddr_in addr = client_addres[handler];
    printf("client disconnect: ip: %s, port: %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    client_addres.erase(handler);
}

void Acceptor::accept_conn(struct evconnlistener* listener,
    evutil_socket_t fd, struct sockaddr* address, int socklen,
    void* ctx) {
    sockaddr_in* addr = (sockaddr_in*)address;
    printf("client connect, ip: %s, port: %d\n", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
    Reactor* reactor = (Reactor*)ctx;
    /* 此时已经接收到了文件描述符，调用Handler处理读写事件 */
    event_base* base = evconnlistener_get_base(listener);
    Handler* handler = new Handler();
    handler->set_base(base);
    handler->set_sockfd(fd);
    handler->set_reactor(reactor);
    handler->init();
    add_client_address(handler, addr);
    ++count;
    // printf("client connection number %d\n", count);
}