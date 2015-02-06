
#ifndef TASK_HPP_
#define TASK_HPP_

#include "Thread.hpp"
#include "Mutex.hpp"


typedef enum {
    TaskStateNotSubmitted,         /* Task initial state */
    TaskStateWaitingOnQueue,       /* Task is waiting on queue state */
    TaskStateBeingProcessed,       /* Task is being processed on thread */
    TaskStateCompleted,            /* Task completed successfully */
    TaskStateFailure               /* Task failure state (stopped, or completed) */
    } TaskStatus_t;

struct TaskData_t {
    unsigned int    procDataLength;
    unsigned int    resDataLength;
    void*           pProcData;
    void*           pResData;
    };

typedef struct TaskData_t   TaskData;


/**
 * @brief:  Task class doing the actual work
 *
 * The Task class is handled by a thread instance for processing 
 * the @ref: Do() method.
 */
class Task {
    public:
        /**
         * @brief:  Default constructor
         */
        Task();

        /**
         * @brief:  Constructor
         */
        Task( TaskData* pData );

        /**
         * @brief:  Default destructor
         */
        ~Task();

        /**
         * @brief:  Set the Id of the task processing thread
         *
         * @param:  pid         Thread ID
         */
        void SetThreadId( ThreadId_t& pid );

        /**
         * @brief:  Get ID of the task processing thread
         *
         * @return              Thread ID
         */
        ThreadId_t& Thread();

        /**
         * @brief:  Sets the state of this task
         *
         * @param:  state       Task state
         */
        void SetTaskStatus( TaskStatus_t state );

        /**
         * @brief:  Get the task's state
         *
         * @return:             State of the task
         */
        TaskStatus_t Status();

        /**
         * @brief:  Wait for given seconds for the task to be completed
         *
         * @param:  timeout     Timeout in seconds
         * @return:             true if comleted, false if still running
         */
        bool WaitSec( unsigned int timeout );

        /**
         * @brief:  Wait for given microseconds for the task to be completed
         *
         * @param:  timeout     Timout in milliseconds
         * @return              true if completed, false if still running
         */
        bool WaitUSec( long timeout );

        /**
         * @brief:  Sets processing data for task
         *
         * @param:  pData       Data pointer to processing data
         */
        void SetData( TaskData* pData ) = 0;

        /**
         * @brief:  Gets processing data from task
         *
         * @return:             pointer to processing data
         */
        TaskData* GetData() = 0;

        /**
         * @brief:  Processing method automatically called by the thread
         */
        virtual void Do() = 0;

    private:
        /** mutex lock exclusively for this task */
        Mutex           _mutex;
        /** Thread ID of the processing thread */
        ThreadId_t      _ownerThread;

    protected:
        /** current state of the task */
        TaskStatus_t    _status;
        /** processing data */
        TaskData*       _pData;
};

#endif /* TASK_HPP_ */

