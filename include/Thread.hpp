
#ifndef THREAD_HPP_
#define THREAD_HPP_

#include <pthread.h>

#ifdef __linux__
    #include "wintypes.h"
#endif
#ifdef _WIN32
    #include <windows.h>
#endif
#include "Task.hpp"
#include "TaskQueue.hpp"
#include "Mutex.hpp"
#include "ThreadEvent.hpp"


typedef enum {
    ThreadStateInit = 0,
    ThreadStateWaiting,
    ThreadStateBusy,
    ThreadStateShuttingDown,
    ThreadStateDown,
    ThreadStateFailure
    } ThreadState_t;


typedef enum {
    ThreadTypeIntervalBased,
    ThreadTypeEventBased
    } ThreadType_t;

typedef DWORD ThreadId_t;


/** conditional values */
#define NO_ERROR                0x00
#define TASK_CREATION           0x01
#define MUTEX_CREATION          0x02
#define THREAD_CREATION         0x04
#define EVENT_CREATION          0x08
#define UNKNOWN                 0x16
#define MEMORY_FAULT            0x32
#define ILLEGAL_EVENT_USAGE     0x64


#define MAX_TASKQUEUE_SIZE       256
#define DEFAULT_TASKQUEUE_SIZE    32


/**
 *
 */
class Thread {
    public:
        Thread(TaskQueue *queue = NULL);
        ~Thread();

        bool Start();
        void Stop();

        void SetThreadType( ThreadType_t type, DWORD idleTime );
        void SetIdleTime( DWORD newIdleTime );

        void AddTask( Task* pTask );
        void RemoveTask( Task* pTask );

        static ThreadId_t ThreadId();
        static bool ThreadIdIsEqual( ThreadId_t* t1, ThreadId_t* t2 );

        size_t GetStackSize();

        bool Event( Task* pTask );

        bool OnTask( void* = NULL);
       
    private:
        friend void* _THREAD( void* pData = NULL );

        bool Process();

        /** thread handle */
        ThreadId_t          _thread;
        /** thread attrinutes */
        pthread_attr_t      _threadAttr;
        /** task queue */
        TaskQueue*          _taskQueue;
        /** thread running state */
        bool                _threadRunning;
        /** */
        ThreadEvent         _event;

    protected:
        /** state of current thread */
        ThreadState_t       _state;
        /** thread condition */
        DWORD               _objCondition;
        /** exclusive thread mutex */
        Mutex               _mutex;
        /** thread type setting */
        ThreadType_t        _threadMode;
        /** idle time value */
        DWORD               _idlTime;
        /** current processing task */
        Task*               _currentTask;
};

#endif /* THREAD_HPP_ */

