#include <bits/stl_queue.h>

#ifndef TASKQUEUE_HPP_
#define TASKQUEUE_HPP_

#include "Task.hpp"


class TaskQueue : public std::queue<Task*>
{
public:
    TaskQueue();
    virtual ~TaskQueue();

private:


};


#endif /* TASKQUEUE_HPP_ */