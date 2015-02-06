
#include "Thread.hpp"

#include <ctime>

using namespace std;

int Sleep( unsigned int mSeconds ) {
#ifdef USE_NANO_SECOND_SLEEP
    struct timespec interval;
    struct timespec reminder;

    mSeconds *= 1000000;
    interval.tv_sec = 0;
    interval.tv_nsec = mSeconds;
    nanosleep( &interval, &reminder );

    #ifdef FORCE_SLEEP_COMPLETED
        if(interval.tv_nsec > 0)
            Sleep((unsigned int)interval.tv_nsec / 1000000);
    #endif
#else
    usleep( mSeconds );
#endif /* USE_NANO_SECOND_SLEEP */

    return 0; // for future use!
}

/* forward declarations */
void* _THREAD( void* pData );


Thread::Thread(TaskQueue *queue)
    : _threadRunning( false )
    , _state( ThreadStateInit )
    , _objCondition( NO_ERROR )
    , _threadMode( ThreadTypeEventBased )
    , _thread( 0L )
    , _taskQueue(queue)
{
    if(!_taskQueue)
        _taskQueue = new TaskQueue( DEFAULT_TASKQUEUE_SIZE );

    if( ! _taskQueue ) {
        _objCondition |= MEMORY_FAULT;
        _state = ThreadStateFailure;
        return;
    }

    if( ! _mutex ) {
        _objCondition |= MUTEX_CREATION;
        _state = ThreadStateFailure;
        return;
    }

    if( ! _event ) {
        _objCondition |= EVENT_CREATION;
        _state = ThreadStateFailure;
        return;
    }

    pthread_attr_init( &_threadAttr );
    pthread_attr_setdetachstate( &_threadAttr, PTHREAD_CREATE_JOINABLE );
}


Thread::~Thread() {
    pthread_attr_destroy( &_threadAttr );
    pthread_exit( NULL );
}


bool Thread::Start() {
    _mutex.Lock();

    // is thread already running?
    if( _threadRunning ) {
        _mutex.Unlock();
        return true;
    }
    _mutex.Unlock();

    // remove THREAD_CREATION flag from condition variable
    if( _objCondition & THREAD_CREATION )
        _objCondition ^= THREAD_CREATION;

    int err = 0;
    err = pthread_create( &_thread, &_threadAttr, _THREAD, (void*)this );

    if( err != 0 ) {
        _objCondition |= THREAD_CREATION;
        _state = ThreadStateFailure;

        // FIXME: error handling here

        return false;
    }
    return true;
}


void Thread::Stop() {
    _mutex.Lock();
    _threadRunning = false;
    _mutex.Unlock();

    Sleep( _idlTime );

    _mutex.Lock();
    while( _state != ThreadStateDown ) {
        _mutex.Unlock();
        Sleep( _idlTime );
        _mutex.Lock();
    }

    _mutex.Unlock();
    Sleep( _idlTime );
}


void Thread::SetThreadType( ThreadType_t type, DWORD idleTime ) {
    _mutex.Lock();

    _threadMode = type;
    _idlTime = idleTime;

    _muttex.Unlock();
    _event.Set();
}


void Thread::SetIdleTime( DWORD newIdleTime ) {
    _mutex.Lock();
    _idlTime = newIdleTime;
    _mutex.Unlock();
}


void Thread::AddTask( Task* pTask ) {
    // TODO: add to task queue
}


void Thread::RemoveTask( Task* pTask ) {
    // TODO: remove from task queue
}


static ThreadId_t Thread::ThreadId() {
    ThreadId_t thisThread = pthread_self();
    return thisThread;
}


static bool Thread::ThreadIdIsEqual( ThreadId_t* t1, ThreadId_t* t2 ) {
    if( pthread_equal( t1, t2 ) != 0 )
        return true;

    return false;
}


size_t Thread::GetStackSize() {
    size_t size;
    this->_mutex.Lock();
    pthread_attr_getstacksize( &_threadAttr, &size );
    this->_mutex.Unlock();
    return size;
}


bool Thread::Event( Task* pTask ) {
    ThreadId_t id;
    _mutex.Lock();

    if( _threadMode == ThreadTypeIntervalBased ) {
        /*  thread is interval based, so this
            is a usage failure and we have to
            mark it illegal and break!          */
        _mutex.Unlock();
        _objCondition |= ILLEGAL_EVENT_USAGE;
        _state = ThreadStateFailure;
        return false;

    } else {
        _mutex.Unlock();
        id = Thread::ThreadId();
        pTask->SetThreadId( id );

        if( ! _taskQueue.PushBack( pTask ) )
            return false;

        pTask->SetTaskStatus( TaskStatusWaitingOnQueue );
        _event.Set();

        return true;
    }
}


void* _THREAD( void* pData ) {
    
    Thread* thread = dynamic_cast<Thread*>( pData );
    
    if( ! thread )
        throw exception();

    // set thread state and ID
    thread->_mutex.Lock();
    thread->_state = ThreadStateWaiting;
    thread->_threadRunning = true;
    thread->_thread = Thread::ThreadId();
    thread->_mutex.Unlock();

    while( true ) {
        // wait for task event if event driven mode
        if( thread->_threadMode == ThreadTypeEventBased ) {
            if( ! thread->_event.Wait() )   // wait until event raised
                break;                      // wait failed, so stop thread
        }

        thread->_mutex.Lock();
        thread->_state = ThreadStateBusy
        thread->_mutex.Unlock();

        // thread shutting down if not running anymore
        thread->_mutex.Lock();
        if( ! thread->_threadRunning ) {
            thread->_state = ThreadStateShuttingDown;
            thread->_mutex.Unlock();
            break;
        }
        thread->_mutex.Unlock();

        /* thread working state */

        // check for task to process
        if( ! thread->_taskQueue->Empty() ) {
            while( ! thread->_taskQueue->Empty() ) {
                // get the next task to be processed
                Task* t;
                
                thread->_mutex.Lock();
                t = thread->_taskQueue->Pop();
                thread->_mutex.Unlock();

                t->SetTaskStatus( TaskStateBeingProcessed );
                t->SetThreadId( thread->_thread );

                // do the work!
                t->Do();
            }

            thread->_mutex.Lock();
            thread->_state = ThreadStateWaiting;

        } else {
            // task queue is empty            
            thread->_mutex.Lock();
            thread->_state = ThreadStateWaiting;
        }            
        thread->_mutex.Unlock();


        if( thread->_threadMode == ThreadTypeEventBased )       
            thread->_event.Reset();
        
        if( thread->_threadMode == ThreadTypeIntervalBased )
            Sleep( thread->_idlTime );

    } // end "while(true)"

    // update thread state and exit thread function
    thread->_mutex.Lock();
    thread->_state = ThreadStateDown;
    thread->_threadRunning = false;
    thread->_mutex.Unlock();

    return (void*) 0;
}


