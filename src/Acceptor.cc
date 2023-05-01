#include "Acceptor.h"


Acceptor::Acceptor(/* args */) {}

Acceptor::~Acceptor() {}

void Acceptor::accept_conn(struct evconnlistener* listener,
    evutil_socket_t fd, struct sockaddr* address, int socklen,
    void* ctx) {
}