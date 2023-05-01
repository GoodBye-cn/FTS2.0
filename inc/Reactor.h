#ifndef _REACTOR_H
#define _REACTOR_H

#include <event2/listener.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unordered_map>

#include <string.h>

class Handler;
class Acceptor;

class Reactor {

public:
    Reactor();
    ~Reactor();

    void add_handler(Handler* handler);
    void remove_handler(Handler* handler);
    void start();

private:
    static void sigquit_cb(evutil_socket_t sig, short what, void* ctx);
    static void accept_conn_cb(struct evconnlistener* listener,
        evutil_socket_t fd, struct sockaddr* address, int socklen,
        void* ctx);
private:
    /* data */
    const char* ip;
    short port;
    sockaddr_in addr;
    event_base* base;
    evconnlistener* listener;
    event* sigquit_event;

    std::unordered_map<Handler*, bool> handlers;
    Acceptor* acceptor;
};


#endif