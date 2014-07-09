
#ifndef THREADEVENT_HPP_
#define THREADEVENT_HPP_

#include <pthread>
#include "Mutex.hpp"


class ThreadEvent {
    public:
        ThreadEvent();
        ~ThreadEvent();

        void Set();
        void Reset();
        bool Wait();

    private:
        Mutex               _mutex;
        pthread_cond_t      _threadCondition;

    protected:

};


#endif /* THREADEVENT_HPP_ */

