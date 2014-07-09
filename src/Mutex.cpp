
#include "Mutex.hpp"
#include <cerrno>
#include <exception>

Mutex::Mutex() {
    int res = 0;
    if( pthread_mutexattr_init( &_mutexAttr ) == ENOMEM ) { throw exception(); }
    res = pthread_mutex_init( &_mutex, &_mutexAttr );

    switch( res ) {
        case EAGAIN:
            break;
        case ENOMEM:
            throw exception();
            break;
        case EPERM:
            break;
        case EBUSY:
            break;
        case EINVAL:
            break;
        default:
            break;
    }
}


Mutex::~Mutex() {
    pthread_mutex_lock( &_mutex );
    pthread_mutex_unlock( &_mutex );

    int res = 0;
    res =  pthread_mutex_destroy( &_mutex );
    
    switch( res ) {
        case EBUSY:
            break;
        case EINVAL:
            break;
        default:
            break;
    }

    if( pthread_mutexattr_destroy( &_mutexAttr ) == EINVAL ) {}
}


void Mutex::Lock() {
    ThreadId_t id = Thread::ThreadId();
    if( Thread::ThreadIdIsEqual( &_owner, &id )
        return;

    pthread_mutex_lock( &_mutex );
    _owner = Thread::ThreadId();
}


int Mutex::TryLock( unsigned int retries, int waittime_sec ) {
    ThreadId_t = Thread::ThreadId();
    if( Thread::ThreadIdIsEqual( &_owner, &id ))
        return -1;

    retries > 0 ? retries : retries = MUTEX_LOCK_DEFAULT_RETRIES;
    waittime_sec > 0 ? waittime_sec : waittime_sec = 1;

    int res = 0;
    for(int n = 0; n < retries; n++ ) {
        if( (res = pthread_mutex_trylock( &_mutex )) == 0 )
            return 0;

        else if( res == EBUSY )
            sleep( waittime_sec ); // change to nanosleep to be interrupt save

        else
            return res;
    }
}


void Mutex::Unlock() {
    ThreadId_t id = Thread::ThreadId();
    if( ! Thread::ThreadIdIsEqual( &_owner, &id ) )
        return;

    pthread_mutex_unlock( &_mutex );
    memset( _owner, 0, sizeof(ThreadId_t) );
}




