#ifndef _HANDLER_H
#define _HANDLER_H

#include <event2/event.h>
#include <memory>

#include "Worker.h"

class Reactor;

class Handler {
public:
    Handler();
    ~Handler();

    void set_base(event_base* base);
    void set_sockfd(int fd);
    void set_self(std::shared_ptr<Handler> self);
    void set_reactor(std::shared_ptr<Reactor> reactor);
    void init();
    void destory();
    bool get_state();
    void init_write_buff(int size);
    void set_write_buff_data(char* data, int size);
    void active_write_event();
    int remove_write_event();
    int add_read_event();
    int add_timeout_event();
    void reset_death_time();
    int remove_read_event();
    void set_filefd(int fd);
    void set_file_stat(int size);
    void finish_request();

private:
    static void read_cb(evutil_socket_t fd, short what, void* arg);
    static void write_cb(evutil_socket_t fd, short what, void* arg);
    static void timeout_cb(evutil_socket_t fd, short what, void* arg);

    static const int READ_BUFF_LEN = 1024;
    static const int WRITE_BUFF_LEN = 1024;

    void init_worker();

private:
    int sockfd;
    event_base* base;
    event* read_event;
    event* write_event;
    event* timeout_event;
    timeval death_time;
    int live_time;
    char read_buff[Handler::READ_BUFF_LEN];
    char* write_buff;
    int filefd;
    int read_buff_index, read_buff_size;
    int write_buff_index, write_buff_size, file_size, file_offset;
    bool working, requesting;
    std::shared_ptr<Worker> worker;
    std::shared_ptr<Reactor> reactor;
    std::shared_ptr<Handler> self;
    int count = 0;
public:
    Mutex write_mutex;
};

#endif