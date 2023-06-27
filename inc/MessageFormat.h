#ifndef _MESSAGEFORMAT_H
#define _MESSAGEFORMAT_H

#define PATHLEN 1024
#define MAX_MSG_LEN 1024

typedef enum { DIR, PATH, DATA } MsgType;

struct Request {
    int length;
    char path[PATHLEN];
};

struct Response {
    int size;
};

struct FileRequest {
    int length;                     // 记录包头的最大长度
    MsgType type;
    char file_path[MAX_MSG_LEN];    // 文件路径
};

struct FileResponse {
    int length;                 // 记录包头的最大长度
    int file_size;              // 文件大小
    char msg[MAX_MSG_LEN];      // 消息
};

#endif