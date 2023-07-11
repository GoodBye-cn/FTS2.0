#ifndef _WORKER_H
#define _WORKER_H

#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory>

#include "Lock.h"
#include "MessageFormat.h"

class Handler;

class Worker {
public:
    enum Status {
        PARSE, OPENFILE, SEND
    };
    enum Line_Status {
        LINE_OK, LINE_OPEN, LINE_BAD
    };

public:
    Worker(/* args */);
    ~Worker();

    void set_buff(char* buff, int buffer_size);
    void set_handler(std::shared_ptr<Handler> handler);
    void work();

private:
    char* buffer;
    int buffer_size;
    struct stat file_stat;
    std::shared_ptr<Handler> handler_tmp;
    FileResponse rsp;
    FileRequest req;
};

#endif