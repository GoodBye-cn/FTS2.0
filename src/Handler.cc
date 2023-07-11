#include "Handler.h"
#include "Reactor.h"
#include "MessageFormat.h"
#include "Guard.h"

#include <cstring>
#include <sys/sendfile.h>

Handler::Handler() {
    this->read_event = NULL;
    this->write_event = NULL;
    this->timeout_event = NULL;
    this->live_time = 5;
    this->read_buff_index = 0;
    this->read_buff_size = 0;
    this->write_buff_index = 0;
    this->write_buff_size = 0;
    this->working = true;
    this->requesting = false;
    this->filefd = -1;
    this->write_buff = NULL;
    // this->worker = new Worker();
    this->worker_tmp = std::make_shared<Worker>();
    // worker->set_handler(this);
    worker_tmp->set_handler(this);
    // worker->set_buff(this->read_buff, Handler::READ_BUFF_LEN);
    worker_tmp->set_buff(this->read_buff, Handler::READ_BUFF_LEN);
    gettimeofday(&death_time, NULL);
    reset_death_time();
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
    if (timeout_event != NULL) {
        event_free(timeout_event);
        timeout_event = NULL;
    }
    // if (worker != NULL) {
    //     delete worker;
    // }
}

void Handler::set_base(event_base* base) {
    this->base = base;
}

void Handler::set_sockfd(int fd) {
    this->sockfd = fd;
}

// void Handler::set_reactor(Reactor* reactor) {
//     this->reactor = reactor;
// }

void Handler::set_reactor(std::shared_ptr<Reactor> reactor) {
    this->reactor_tmp = reactor;
}


void Handler::init() {
    read_event = event_new(base, sockfd, EV_READ | EV_PERSIST, read_cb, this);
    write_event = event_new(base, sockfd, EV_WRITE | EV_PERSIST | EV_ET, write_cb, this);
    timeout_event = event_new(base, -1, EV_TIMEOUT, timeout_cb, this);
    // 添加事件
    event_add(read_event, NULL);
    add_timeout_event();
}

void Handler::destory() {
    // 删除事件
    event_del(read_event);
    event_del(write_event);
    event_del(timeout_event);
    // 关闭描述符
    if (sockfd > 0) {
        close(sockfd);

    }
    if (filefd > 0) {
        close(filefd);
    }
    // 添加到删除队列
    reactor_tmp->add_remove_list(std::make_shared<Handler>(this));
    printf("add handler to remove list and remove read write event\n");
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
    event_add(write_event, NULL);
    // 当write_event就是激活状态的时候，不会再次激活
    event_active(write_event, EV_WRITE, 1);
    printf("activate write event\n");
}

int Handler::remove_write_event() {
    return event_del(write_event);
}

int Handler::add_read_event() {
    return event_add(read_event, NULL);
}

int Handler::remove_read_event() {
    return event_del(read_event);
}

int Handler::add_timeout_event() {
    timeval tv;
    tv.tv_sec = live_time;
    tv.tv_usec = 0;
    return event_add(timeout_event, &tv);
}

/**
 * 重置死亡时间，现有的死亡时间基础上加 live_time s
 */
void Handler::reset_death_time() {
    timeval tv;
    tv.tv_sec = live_time;
    tv.tv_usec = 0;
    timeradd(&death_time, &tv, &death_time);
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

void Handler::finish_request() {
    remove_write_event();
    close(filefd);
    filefd = -1;
    delete[] write_buff;
    write_buff = NULL;
    add_read_event();
    requesting = false;
}

void Handler::read_cb(evutil_socket_t fd, short what, void* arg) {
    // 读取为0关闭连接，直接释放资源
    printf("read event callback\n");
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
            if (handler->read_buff_index == value) {
                // 开始处理任务
                handler->reactor_tmp->get_threadpool()->append(handler->worker_tmp.get());
                // 测试单线程的时候使用
                // handler->worker->work();
                handler->remove_read_event();
                handler->read_buff_index = 0;
                handler->requesting = true;
                break;
            }
        }
    }
}

void Handler::write_cb(evutil_socket_t fd, short what, void* arg) {
    // 写出错，关闭连接，直接释放资源
    // 需要加锁对 write_buff_index 和 write_buff_size 操作的时候
    printf("write event callback\n");
    Handler* handler = (Handler*)arg;
    MutexGuard mutex_guard(handler->write_mutex);
    while (true) {
        // 发送Response，buffer中的内容为Response
        if (handler->write_buff != NULL && handler->write_buff_index < handler->write_buff_size) {
            char* buff = handler->write_buff;
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
            continue;
        }
        // 请求路径失败，buffer发送完直接关闭这次请求
        if (handler->write_buff != NULL && handler->filefd < 0) {
            handler->finish_request();
        }
        // 回复的头发送完毕，开始发送文件
        if (handler->write_buff != NULL && handler->filefd > 0) {
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
                break;
            }
            handler->file_offset += bytes;
            if (handler->file_offset == handler->file_size) {
                // 文件发送完毕，清除buff，关闭文件，移出写事件
                handler->finish_request();
                break;
            }
        }
        else {
            ++handler->count;
            printf("write ok but not send data %d\n", handler->count);
            break;
        }
    }
}

void Handler::timeout_cb(evutil_socket_t fd, short what, void* arg) {
    // 比较Handler的死亡时间和现在的时间
    Handler* handler = (Handler*)arg;
    timeval now;
    gettimeofday(&now, NULL);
    // 如果未超时，等待下一次连接
    if (timercmp(&now, &(handler->death_time), < )) {
        handler->add_timeout_event();
        return;
    }
    // 如果超时，但还有任务，加时
    if (handler->requesting) {
        handler->add_timeout_event();
        handler->reset_death_time();
        return;
    }
    printf("timeout and close connection\n");
    handler->destory();
    handler->working = false;
    return;
}
