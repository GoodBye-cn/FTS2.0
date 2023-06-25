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
    memset(&req, 0, sizeof(FileRequest));
    memset(&rsp, 0, sizeof(FileResponse));

    memcpy(&req, buffer, sizeof(FileRequest));
    req.file_path[req.length - sizeof(MsgType) - sizeof(int)] = 0;
    int filefd = open(req.file_path, O_RDONLY);
    if (-1 == filefd) {
        rsp.file_size = -1;
        sprintf(rsp.msg, "file path error");
        rsp.length = sizeof(int) + sizeof(int) + strlen(rsp.msg);
    }
    else {
        stat(req.file_path, &file_stat);
        rsp.file_size = file_stat.st_size;
        sprintf(rsp.msg, "this is file");
        rsp.length = sizeof(int) + sizeof(int) + strlen(rsp.msg);
        handler->set_file_stat(file_stat.st_size);
    }
    handler->write_mutex.lock();
    handler->set_filefd(filefd);
    handler->init_write_buff(rsp.length);
    handler->set_write_buff_data((char*)&rsp, rsp.length);
    handler->write_mutex.unlock();
    handler->active_write_event();
}
