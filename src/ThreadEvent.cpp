
#include "ThreadEvent.cpp"
#include <cerrno>

ThreadEvent::ThreadEvent() {
    pthread_cond_init( &_threadCondition, NULL );
}


ThreadEvent::~ThreadEvent() {
    pthread_cond_destroy( &_threadCondition );
}


void ThreadEvent::Set() {
    _mutex.Lock();
    _mutex_Unlock();
    pthread_cond_signal( &_threadCondition );
}


void ThreadEven::Reset() {
    // for future use
}


bool ThreadEvent::Wait() {
    _mutex.Lock();
    if( pthread_cond_wait( &_threadCondition, &_mutex._mutex ) == EINVAL )
        return false;

    return true;
}

