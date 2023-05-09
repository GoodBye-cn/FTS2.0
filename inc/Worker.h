#ifndef _WORKER_H
#define _WORKER_H

#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "MessageFormat.h"

class Handler;

class Worker {
public:
    enum Status { PARSE, OPENFILE, SEND };
    enum Line_Status { LINE_OK, LINE_OPEN, LINE_BAD };

public:
    Worker(/* args */);
    ~Worker();

    void set_buff(char* buff, int buffer_size);
    void set_handler(Handler* handler);
    Status get_status();

    void work();

private:
    char* buffer;
    int buffer_size;
    Status status;

    char* path;
    struct stat file_stat;
    Handler* handler;
    Request resq;
    Response resp;
};

#endif