
#ifndef MUTEX_HPP_
#define MUTEX_HPP_

#include <pthread.h>
#include "Thread.hpp"
#include "ThreadEvent.hpp"

#define MUTEX_LOCK_DEFAULT_RETRIES  5

class ThreadEvent;

class Mutex {    
    friend ThreadEvent;
    
    public:
        Mutex();
        ~Mutex();

        void Lock();
        int TryLock( unsigned int retries = MUTEX_LOCK_DEFAULT_RETRIES, int waittime_sec = 1 );
        void Unlock();

    private:
        pthread_mutex_t         _mutex;
        pthread_mutexattr_t     _mutexAttr;
        ThreadId_t              _owner;

};


#endif /* MUTEX_HPP_ */

