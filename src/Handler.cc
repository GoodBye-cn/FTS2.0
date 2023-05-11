#include "Handler.h"
#include "Reactor.h"
#include "MessageFormat.h"

#include <cstring>
#include <sys/sendfile.h>

Handler::Handler() {
    this->read_event = NULL;
    this->write_event = NULL;
    this->read_buff_index = 0;
    this->read_buff_size = 0;
    this->write_buff_index = 0;
    this->write_buff_size = 0;
    this->working = true;
    this->filefd = -1;
    this->worker = new Worker();
    worker->set_handler(this);
    worker->set_buff(this->read_buff, Handler::READ_BUFF_LEN);
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

    if (worker != NULL) {
        delete worker;
    }
}

void Handler::set_base(event_base* base) {
    this->base = base;
}

void Handler::set_sockfd(int fd) {
    this->sockfd = fd;
}

void Handler::set_reactor(Reactor* reactor) {
    this->reactor = reactor;
}

void Handler::init() {
    read_event = event_new(base, sockfd, EV_READ | EV_PERSIST, read_cb, this);
    write_event = event_new(base, sockfd, EV_WRITE | EV_PERSIST, write_cb, this);
    // 添加事件
    event_add(read_event, NULL);
    event_add(write_event, NULL);
}

void Handler::destory() {
    // 删除事件
    event_del(read_event);
    event_del(write_event);
    reactor->add_remove_list(this);
}

bool Handler::get_state() {
    return this->working;
}

// 初始化buffer
void Handler::init_write_buff(int size) {
    if (write_buff != NULL) {
        delete[] write_buff;
    }
    this->write_buff = new char[size];
    this->write_buff_index = 0;
    this->write_buff_size = size;
}

// 手动激活事件
void Handler::active_write_event() {
    event_active(write_event, EV_WRITE, 0);
}

void Handler::set_filefd(int fd) {
    this->filefd = fd;
}

// 设置write_buffer中的数据
void Handler::set_write_buff_data(char* data, int size) {
    memcpy(write_buff, data, size);
    write_buff_size = size;
    write_buff_index = 0;
}

void Handler::set_file_stat(int size) {
    file_offset = 0;
    file_size = size;
}

void Handler::read_cb(evutil_socket_t fd, short what, void* arg) {
    // 读取为0关闭连接，直接释放资源
    Handler* handler = (Handler*)arg;
    char* buff = handler->read_buff;
    int index = handler->read_buff_index;
    size_t bytes = 0;
    while (true) {
        bytes = recv(fd, buff + index, Handler::READ_BUFF_LEN - index, 0);
        if (-1 == bytes) {
            // 缓冲区数据读取完毕
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                break;
            }
            handler->destory();
            handler->working = false;
        }
        // 客户端关闭了连接
        if (0 == bytes) {
            handler->destory();
            handler->working = false;
            break;
        }
        handler->read_buff_index += bytes;
        // 请求读取完在处理
        int value;
        if (handler->read_buff_index >= sizeof(int)) {
            memcpy(&value, buff, sizeof(int));
            if (handler->read_buff_index == value + sizeof(int)) {
                // 开始处理任务
                // handler->reactor->get_threadpool()->append(handler->worker);
                handler->worker->work();
                break;
            }
        }
    }
}

void Handler::write_cb(evutil_socket_t fd, short what, void* arg) {
    // 写出错，关闭连接，直接释放资源
    Handler* handler = (Handler*)arg;
    while (true) {
        if (handler->write_buff_index < handler->write_buff_size) {
            char* buff = handler->write_buff;
            Response tmp;
            memcpy(&tmp, buff, handler->write_buff_size);
            printf("file size: %d", tmp.size);
            int buff_index = handler->write_buff_index;
            int buff_size = handler->write_buff_size;
            size_t bytes = 0;
            bytes = send(fd, buff + buff_index, buff_size - buff_index, 0);
            if (-1 == bytes) {
                // 写缓冲区已满
                if (errno == EWOULDBLOCK || errno == EAGAIN) {
                    break;
                }
                handler->destory();
                handler->working = false;
            }
            handler->write_buff_index += bytes;
        }
        if (handler->filefd > 0) {
            // 使用sendfile函数
            off_t off = handler->file_offset;
            ssize_t bytes = 0;
            bytes = sendfile(handler->sockfd, handler->filefd, &off, handler->file_size - handler->file_offset);
            if (-1 == bytes) {
                if (errno == EAGAIN) {
                    break;
                }
                handler->destory();
                handler->working = false;
            }
            handler->file_offset += bytes;
            if (handler->file_offset == handler->file_size) {
                close(handler->filefd);
                handler->filefd = -1;
                break;
            }
        }
        else {
            break;
        }
    }
}

void Handler::event_cb(struct bufferevent* bev, short what, void* ctx) {}

void Handler::func_cb(evutil_socket_t fd, short what, void* arg) {
    // 读取 fd 中的内容到 buffer
    if (what & EV_READ) {

    }
    // 将 buffer 中的内容写道 fd
    if (what & EV_WRITE) {

    }
}
