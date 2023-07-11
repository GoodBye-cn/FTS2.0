#include "Reactor.h"
#include "Acceptor.h"
#include "Worker.h"


#include <signal.h>

Reactor::Reactor() {
    this->port = 8888;
    this->ip = "0.0.0.0";
    this->clear_client_data_slot = 5;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    this->base = event_base_new();
    // acceptor = new Acceptor();
    acceptor_tmp = std::make_unique<Acceptor>();
}

Reactor::~Reactor() {
    event_free(sigquit_event);
    event_base_free(base);
    evconnlistener_free(listener);
    // delete acceptor;
}

void Reactor::set_self(std::shared_ptr<Reactor> self) {
    this->self = self;
}

void Reactor::start() {
    /* process signal quit */
    sigquit_event = event_new(base, SIGQUIT, EV_SIGNAL | EV_PERSIST, sigquit_cb, NULL);
    event_add(sigquit_event, NULL);

    timer_event = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, timeout_cb, this);
    evutil_timerclear(&time_slot);
    time_slot.tv_sec = clear_client_data_slot;
    time_slot.tv_usec = 0;
    event_add(timer_event, &time_slot);

    listener = evconnlistener_new_bind(base, accept_conn_cb, this, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, (sockaddr*)&addr, sizeof(addr));
    if (listener == NULL) {
        perror("Couldn't create listener");
        return;
    }
    event_base_dispatch(base);
}


// void Reactor::add_handler(std::shared_ptr<Handler> handler) {
//     handlers_tmp.insert(handler);
// }


void Reactor::remove_handler() {
    int num = remove_list_tmp.size();
    if (remove_list_tmp.size() == 0) {
        return;
    }
    for (int i = 0; i < remove_list_tmp.size(); i++) {
        printf("use count of handler: %ld\n", remove_list_tmp[i].use_count());
        acceptor_tmp->remove_client_address(remove_list_tmp[i]);
    }
    // printf("use count of handler: %ld\n", remove_list_tmp[i].use_count());
    remove_list_tmp.clear();
    printf("delete handler number: %d\n", num);
}


// 将要销毁的Handler放到销毁队列中
void Reactor::add_remove_list(std::shared_ptr<Handler> handler) {
    remove_list_tmp.push_back(handler);
}

void Reactor::add_timer() {
    evutil_timerclear(&time_slot);
    time_slot.tv_sec = 5;
    time_slot.tv_usec = 0;
    event_add(timer_event, &time_slot);
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

void Reactor::timeout_cb(evutil_socket_t sig, short what, void* ctx) {
    Reactor* reactor = (Reactor*)ctx;
    printf("time out event and start delete handler\n");
    reactor->remove_handler();
}

/**
 * @brief 接收新的客户端连接
 * @details 通过Reactor指针，将参数传递给Acceptor的连接处理函数中
 */
void Reactor::accept_conn_cb(struct evconnlistener* listener,
                             evutil_socket_t fd, struct sockaddr* address, int socklen,
                             void* ctx) {
    Reactor* reactor = (Reactor*)ctx;
    reactor->acceptor_tmp->accept_conn(listener, fd, address, socklen, reactor->self);
}