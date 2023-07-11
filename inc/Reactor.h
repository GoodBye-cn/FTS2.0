#ifndef _REACTOR_H
#define _REACTOR_H

#include <event2/listener.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <set>
#include <vector>
#include <string.h>
#include <memory>

#include "Threadpool.h"
#include "Worker.h"


class Handler;
class Acceptor;

class Reactor {
public:
    Reactor();
    ~Reactor();

    // void add_handler(Handler* handler);
    void add_handler(std::shared_ptr<Handler> handler);
    void remove_handler();
    // void remove_handler(Handler* handler);
    void remove_handler(std::shared_ptr<Handler> handler);

    void start();
    void add_timer();
    // void add_remove_list(Handler* handler);
    void add_remove_list(std::shared_ptr<Handler> handler);
    void set_threadpool(Threadpool<Worker>* tp);
    Threadpool<Worker>* get_threadpool();


private:
    static void sigquit_cb(evutil_socket_t sig, short what, void* ctx);
    static void timeout_cb(evutil_socket_t sig, short what, void* ctx);
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
    event* timer_event;
    // std::vector<Handler*> remove_list;
    std::vector<std::shared_ptr<Handler>> remove_list_tmp;
    // std::set<Handler*> handlers;
    std::set<std::shared_ptr<Handler>> handlers_tmp;
    // Acceptor* acceptor;
    std::unique_ptr<Acceptor> acceptor_tmp;
    Threadpool<Worker>* threadpool;
    timeval time_slot;
    int clear_client_data_slot;
    std::shared_ptr<Reactor> self;
};


#endif