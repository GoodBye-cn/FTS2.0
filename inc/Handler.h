#ifndef _HANDLER_H
#define _HANDLER_H

#include <event2/event.h>

class Handler {
public:
    Handler();
    ~Handler();

    void set_base(event_base* base);
    void set_fd(int fd);
    void init();
private:
    static void read_cb(struct bufferevent* bev, void* ctx);
    static void write_cb(struct bufferevent* bev, void* ctx);
    static void event_cb(struct bufferevent* bev, short what, void* ctx);
    static void func_cb(evutil_socket_t fd, short what, void* arg);

    static const int READ_BUFF_LEN = 1024;
    static const int WRITE_BUFF_LEN = 1024;

private:
    /* data */
    int fd;
    event_base* base;
    event* read_event;
    event* write_event;
    char read_buff[Handler::READ_BUFF_LEN];
    char write_buff[Handler::WRITE_BUFF_LEN];
};

#endif