
#include "Task.hpp"

#include <ctime>


Task::Task() 
    : _pData( NULL ) 
{
    _status = TaskStateNotSubmitted;
    memset( &_ownerThread, 0, sizeof(ThreadId_t) );
}


Task::Task( TaskData* pData ) 
    : _pData( pData ) 
{
    _status = TaskStateNotSubmitted;
    memset( &_ownerThread, 0, sizeof(ThreadId_t) );
}


Task::~Task() {
    // get mutex lock to wait until all locks
    // are released, before destroying the object.
    this->_mutex.Lock();
    this->_mutex.Unlock();
}


void Task::SetThreadId( ThreadId_t& pid ) {
    this->_mutex.Lock();
    memcpy( &_ownerThread, pid, sizeof(ThreadId_t) );
    this->_mutex.Unlock();
}


ThreadId_t& Task::Thread() {
    return this->_ownerThread;
}


void Task::SetTaskStatus( TaskStatus_t state ) {
    this->_mutex.Lock();
    memcpy( &_status, &state, sizeof(TaskStatus_t) );
    this->_mutex.Unlock();
}


TaskStatus_t Task::Status() {
    return this->_status;
}


bool Task::WaitSec( unsigned int timeout ) {
    if( Status() != TaskStateCompleted && timeout > 0 ) {
        struct timespec req, rem;
        int res = 0;
        
        req.tv_sec = timeout;
        req.tv_nsec = 0;

        res = nanosleep( &req, &rem );
        while( res == EINTR ) {
            // task has been interrupted, so proceed with remaining time
            req = rem;
            res = nanosleep( &req, &rem );
        }
    }

    if( Status() == TaskStateCompleted )
        return true;

    return false;
}


bool Task::WaitUSec( long timeout ) {
    struct timespec req, rem;
    int res = 0;

    req.tv_sec = 0;
    req.tv_nsec = timeout;

    if( Status() != TaskStateCompleted && req.tv_nsec > 0 ) {
        res = nanosleep( &req, &rem );
        while( res == EINTR ) {
            // task has been interrupted, so proceed with remaining time
            req = rem;
            res = nanosleep( &req, &rem );
        }
    }

    if( Status() == TaskStateCompleted )
        return true;

    return false;

}


void Task::SetData( TaskData* pData ) {
    this->_mutex.Lock();
    this->_pData = pData;
    this->_mutex.Unlock();
}


TaskData* Task::GetData() {
    return this->_pData;
}

