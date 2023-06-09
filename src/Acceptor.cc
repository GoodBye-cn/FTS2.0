#include "Acceptor.h"
#include "Handler.h"
#include "Reactor.h"

Acceptor::Acceptor() {
    count = 0;
}

Acceptor::~Acceptor() {}

void Acceptor::add_client_address(std::shared_ptr<Handler> handler, sockaddr_in addr) {
    client_address[handler] = addr;
}

void Acceptor::remove_client_address(std::shared_ptr<Handler> handler) {
    sockaddr_in addr = client_address[handler];
    printf("client disconnect: ip: %s, port: %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    client_address.erase(handler);
}

void Acceptor::accept_conn(struct evconnlistener* listener,
                           evutil_socket_t fd, struct sockaddr* address, int socklen,
                           std::shared_ptr<Reactor> reactor) {
    sockaddr_in* addr =  (sockaddr_in*)address;
    printf("client connect, ip: %s, port: %d\n", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
    /* 此时已经接收到了文件描述符，调用Handler处理读写事件 */
    event_base* base = evconnlistener_get_base(listener);
    std::shared_ptr<Handler> handler = std::make_shared<Handler>();
    handler->set_self(handler);
    handler->set_base(base);
    handler->set_sockfd(fd);
    handler->set_reactor(reactor);
    handler->init();
    add_client_address(handler, *addr);
}