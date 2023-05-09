#ifndef _MESSAGEFORMAT_H
#define _MESSAGEFORMAT_H

#define PATHLEN 1024

struct Request {
    int length;
    char path[PATHLEN];
};

struct Response {
    int size;
};

#endif