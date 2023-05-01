#ifndef _HANDLER_H
#define _HANDLER_H

class Handler {
public:
    Handler();
    ~Handler();
private:
    static void read_cb(struct bufferevent* bev, void* ctx);
    static void write_cb(struct bufferevent* bev, void* ctx);
    static void event_cb(struct bufferevent* bev, short what, void* ctx);

private:
    /* data */
};

#endif