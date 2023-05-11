#include "Worker.h"
#include "Handler.h"
#include "string.h"
#include "Guard.h"
#include "Lock.h"

Worker::Worker() {}

Worker::~Worker() {}

void Worker::set_buff(char* buff, int buffer_size) {
    this->buffer = buff;
    this->buffer_size = buffer_size;
}

void Worker::set_handler(Handler* handler) {
    this->handler = handler;
}

void Worker::work() {
    memcpy(&resq, buffer, sizeof(Request));
    resq.path[resq.length] = 0;
    int filefd = open(resq.path, O_RDONLY);
    if (-1 == filefd) {
        resp.size = -1;
    }
    else {
        stat(resq.path, &file_stat);
        resp.size = file_stat.st_size;
        handler->set_file_stat(file_stat.st_size);
    }
    // MutexGuard mutex_guard(handler->write_mutex);
    handler->write_mutex.lock();
    handler->set_filefd(filefd);
    handler->init_write_buff(sizeof(Response));
    handler->set_write_buff_data((char*)&resp, sizeof(Response));
    handler->write_mutex.unlock();
    handler->active_write_event();
}
