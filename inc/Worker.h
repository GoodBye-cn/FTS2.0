#ifndef _WORKER_H
#define _WORKER_H

class Worker {
public:
    Worker(/* args */);
    ~Worker();

    void set_buff(char* buff);
private:
    char* buffer;
};

#endif