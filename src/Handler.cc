#include "Handler.h"


Handler::Handler() {}
Handler::~Handler() {
    event_free(read_event);
    read_event = NULL;
    event_free(write_event);
    write_event = NULL;
}

void Handler::set_base(event_base* base) {
    this->base = base;
}

void Handler::set_fd(int fd) {
    this->fd = fd;
}

void Handler::init() {
    read_event = event_new(base, fd, EV_READ | EV_PERSIST, func_cb, this);
    write_event = event_new(base, fd, EV_WRITE | EV_PERSIST, func_cb, this);
    event_add(read_event, NULL);
    event_add(write_event, NULL);
    // 删除事件
    event_del(read_event);
}

void Handler::read_cb(struct bufferevent* bev, void* ctx) {}
void Handler::write_cb(struct bufferevent* bev, void* ctx) {}
void Handler::event_cb(struct bufferevent* bev, short what, void* ctx) {}
void Handler::func_cb(evutil_socket_t fd, short what, void* arg) {
    // 读取 fd 中的内容到 buffer
    if (what & EV_READ) {

    }
    // 将 buffer 中的内容写道 fd
    if (what & EV_WRITE) {

    }
}
