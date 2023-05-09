#ifndef _LOCK_H
#define _LOCK_H

#include <pthread.h>
#include <exception>

class Mutex {
public:
    Mutex() {
        if (pthread_mutex_init(&mutex, NULL) != 0) {
            throw std::exception();
        }
    }

    ~Mutex() {
        pthread_mutex_destroy(&mutex);
    }

    int lock() {
        return pthread_mutex_lock(&mutex);
    }

    int unlock() {
        return pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_t* get() {
        return &mutex;
    }

private:
    pthread_mutex_t mutex;
};

class Cond {
public:
    Cond() {
        if (pthread_cond_init(&cond, NULL) != 0) {
            throw std::exception();
        }
    }

    ~Cond() {
        pthread_cond_destroy(&cond);
    }

    int wait(pthread_mutex_t* mutex) {
        return pthread_cond_wait(&cond, mutex);
    }

    int timewait(pthread_mutex_t* mutex, timespec t) {
        return pthread_cond_timedwait(&cond, mutex, &t);
    }

    int signal() {
        return pthread_cond_signal(&cond);
    }

    int broadcast() {
        return pthread_cond_broadcast(&cond);
    }

private:
    pthread_cond_t cond;
};

#endif