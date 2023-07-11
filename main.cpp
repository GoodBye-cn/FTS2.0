#include <iostream>
#include <memory>
#include "Reactor.h"
#include "Threadpool.h"

int main(int, char**) {
    std::shared_ptr<Reactor> reactor = std::make_shared<Reactor>();
    reactor->set_self(reactor);
    Threadpool<Worker> tp(2, 100);
    reactor->set_threadpool(&tp);
    reactor->start();
    std::cout << "Hello, world!\n";
}
