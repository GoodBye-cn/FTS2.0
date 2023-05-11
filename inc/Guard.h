#ifndef _GUARD_H
#define _GUARD_H

#include <pthread.h>
#include "Lock.h"

class MutexGuard {
public:
    MutexGuard(Mutex& mutex) :m_mutex(mutex) {
        m_mutex = mutex;
        int res = 0;
        if ((res = m_mutex.lock()) != 0) {
            std::exception();
        }
        printf("mutex lock result %d\n", res);
    }
    ~MutexGuard() {
        m_mutex.unlock();
    }
private:
    Mutex& m_mutex;
};

#endif