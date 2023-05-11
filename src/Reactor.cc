#include "Reactor.h"
#include "Acceptor.h"
#include "Worker.h"
#include <signal.h>

Reactor::Reactor() {
    this->port = 8888;
    this->ip = "0.0.0.0";
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    this->base = event_base_new();
    acceptor = new Acceptor();
}

Reactor::~Reactor() {
    event_free(sigquit_event);
    event_base_free(base);
    evconnlistener_free(listener);
    delete acceptor;
}

void Reactor::start() {
    /* process signal quit */
    sigquit_event = event_new(base, SIGQUIT, EV_SIGNAL | EV_PERSIST, sigquit_cb, NULL);
    event_add(sigquit_event, NULL);
    listener = evconnlistener_new_bind(base, accept_conn_cb, this, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, (sockaddr*)&addr, sizeof(addr));
    if (listener == NULL) {
        perror("Couldn't create listener");
        return;
    }
    event_base_dispatch(base);
}

void Reactor::add_handler(Handler* handler) {
    handlers.insert(handler);
}

void Reactor::remove_handler() {
    int num = remove_list.size();
    if (remove_list.size() == 0) {
        return;
    }
    for (int i = 0; i < remove_list.size(); i++) {
        handlers.erase(remove_list[i]);
    }
    remove_list.clear();
    printf("delete handler number: %d", num);
}

void Reactor::remove_handler(Handler* handler) {
    handlers.erase(handler);
}

void Reactor::add_remove_list(Handler* handler) {
    remove_list.push_back(handler);
}


void Reactor::set_threadpool(Threadpool<Worker>* tp) {
    this->threadpool = tp;
}

Threadpool<Worker>* Reactor::get_threadpool() {
    return threadpool;
}


void Reactor::sigquit_cb(evutil_socket_t sig, short what, void* ctx) {
    printf("SIGQUIT\n");
}

/**
 * @brief 接收新的客户端连接
 * @details 通过Reactor指针，将参数传递给Acceptor的连接处理函数中
 */
void Reactor::accept_conn_cb(struct evconnlistener* listener,
    evutil_socket_t fd, struct sockaddr* address, int socklen,
    void* ctx) {
    Reactor* reactor = (Reactor*)ctx;
    reactor->acceptor->accept_conn(listener, fd, address, socklen, ctx);
}