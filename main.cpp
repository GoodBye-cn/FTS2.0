#include <iostream>
#include "Reactor.h"
#include "Threadpool.h"

int main(int, char**) {
    Reactor reactor;
    Threadpool<Worker> tp(2, 100);
    reactor.set_threadpool(&tp);
    reactor.start();
    std::cout << "Hello, world!\n";
}
