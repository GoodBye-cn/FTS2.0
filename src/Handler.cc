#include "Handler.h"


Handler::Handler(/* args */) {}

Handler::~Handler() {}

void Handler::read_cb(struct bufferevent* bev, void* ctx) {}
void Handler::write_cb(struct bufferevent* bev, void* ctx) {}
void Handler::event_cb(struct bufferevent* bev, short what, void* ctx) {}