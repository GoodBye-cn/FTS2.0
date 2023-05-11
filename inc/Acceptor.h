#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include <event2/listener.h>
#include <unordered_map>
#include <arpa/inet.h>
#include <netinet/in.h>

class Handler;

class Acceptor {
public:
    Acceptor();
    ~Acceptor();
    void accept_conn(struct evconnlistener* listener,
        evutil_socket_t fd, struct sockaddr* address, int socklen,
        void* ctx);
    void add_client_address(Handler* handler, sockaddr_in* addr);
    void remove_client_address(Handler* handler);

private:
    std::unordered_map <Handler*, sockaddr_in> client_addres;
};


#endif