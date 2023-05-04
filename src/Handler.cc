#include "Handler.h"


Handler::Handler() {
    read_event = NULL;
    write_event = NULL;
}
Handler::~Handler() {
    if (read_event != NULL) {
        event_free(read_event);
        read_event = NULL;
    }
    if (write_event != NULL) {
        event_free(write_event);
        write_event = NULL;
    }
}

void Handler::set_base(event_base* base) {
    this->base = base;
}

void Handler::set_fd(int fd) {
    this->fd = fd;
}

void Handler::init() {
    read_event = event_new(base, fd, EV_READ | EV_PERSIST, read_cb, this);
    write_event = event_new(base, fd, EV_WRITE | EV_PERSIST, write_cb, this);
    // 添加事件
    event_add(read_event, NULL);
    event_add(write_event, NULL);
    // 删除事件
    event_del(read_event);
    event_del(write_event);
}

void Handler::read_cb(evutil_socket_t fd, short what, void* arg) {}
void Handler::write_cb(evutil_socket_t fd, short what, void* arg) {}
void Handler::func_cb(evutil_socket_t fd, short what, void* arg) {
    // 读取 fd 中的内容到 buffer
    if (what & EV_READ) {

    }
    // 将 buffer 中的内容写道 fd
    if (what & EV_WRITE) {

    }
}
