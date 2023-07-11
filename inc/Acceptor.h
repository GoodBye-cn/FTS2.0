#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include <event2/listener.h>
#include <unordered_map>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <memory>

class Handler;
class Reactor;

class Acceptor {
public:
    Acceptor();
    ~Acceptor();
    void accept_conn(struct evconnlistener* listener,
                     evutil_socket_t fd, struct sockaddr* address, int socklen,
                     std::shared_ptr<Reactor> reactor);
    // void add_client_address(Handler* handler, sockaddr_in* addr);
    void add_client_address(std::shared_ptr<Handler> handler, std::shared_ptr<sockaddr_in> addr);
    // void remove_client_address(Handler* handler);
    void remove_client_address(std::shared_ptr<Handler> handler);


private:
    // std::unordered_map <Handler*, sockaddr_in> client_addres;
    std::unordered_map <std::shared_ptr<Handler>, std::shared_ptr<sockaddr_in>> client_address_tmp;

    unsigned int count;
};


#endif