#ifndef _GUARD_H
#define _GUARD_H

#include <pthread.h>
#include "Lock.h"

class MutexGuard {
public:
    MutexGuard(Mutex& mutex) :m_mutex(mutex) {
        m_mutex = mutex;
        m_mutex.lock();
    }
    ~MutexGuard() {
        m_mutex.unlock();
    }
private:
    Mutex& m_mutex;
};

#endif