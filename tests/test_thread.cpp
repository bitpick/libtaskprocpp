
#include "Thread.hpp"
#include "Task.hpp"

using namespace std;

class TestTask : public Task
{
public:
    TestTask() { super(); }
    virtual ~TestTask() {};



private:

};

int main(int argc, char **argv)
{
    Thread *thread = new Thread();
    thread->SetThreadType(ThreadTypeIntervalBased, 10);

    TestTask *task = new TestTask();

    thread->AddTask(task);
    thread->Start();
}