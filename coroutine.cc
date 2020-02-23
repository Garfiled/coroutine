#include <stdio.h>
#include <queue>
#include <iostream>

#define go(x,y) runtime->newproc(x,y)

class Coroutine;
class Context;
class Runtime;

Coroutine* curr_co;
void* sched_sp;
Runtime* runtime;

class Context
{
public:
    int edi;
    int esi;
    int ebx;
    int ebp;
    int eip;
};

class Coroutine
{
public:
    int id;
    char* stack_alloc;
    void* stack_base;
    void* func;
    void* argv;
    void* next;
    void* esp;
    int stat;

    Coroutine(int _id,int _size,void* _func,void* _argv)
    {
        id = _id;
        stack_alloc = new char[_size];
        stack_base = &stack_alloc[_size];
        func = _func;
        argv = _argv;
        // 初始化堆栈
        *((void**)stack_base-1) = _argv;
        *((int*)stack_base-2) = 0;
        *((int*)stack_base-3) = 0;
        *((int*)stack_base-4) = 0;
        *((int*)stack_base-5) = 0;
        esp = (int*)stack_base-5;
        next = _func;
        stat = 0;
    }
};

extern "C" int corun();
extern "C" void yield();

class Runtime
{
public:
    std::queue<Coroutine*> q;
    int idx;

    Runtime():idx(0)
    {

    };

    void newproc(void (*func)(void*),void* argv);
    void enq(Coroutine* c);
    Coroutine* deq();

    void schedule();
};

void Runtime::newproc(void (*func)(void*), void *argv)
{
    Coroutine* c = new Coroutine(this->idx,1024*1024,(void*)func,argv);
    this->enq(c);
}

void Runtime::enq(Coroutine *c)
{
    this->q.push(c);
}

Coroutine* Runtime::deq()
{
    if (this->q.empty())
        return nullptr;
    else {
        Coroutine* c = this->q.front();
        this->q.pop();
        return c;
    }

}

void Runtime::schedule()
{
    int ret;
    while (true)
    {
        Coroutine* c = this->deq();
        if ( c== nullptr) {
            std::cout << "schedule queue empty" << std::endl;
            break;
        }
        curr_co = c;
        ret = corun();
        if (ret==0)
        {
            delete c->stack_alloc;
            delete c;
        } else {
            if (c->stat == 0)
                c->stat = 1;
            this->enq(c);
        }
    }

}


void foo(void* argv)
{
    int* a = (int*)argv;
    std::cout << "foo param" << *a << std::endl;
    yield();
    std::cout << "foo exit" << std::endl;
}

void hi(void* argv)
{
    char* a= (char*)argv;
    std::cout << "hay" << std::endl;
    yield();
    std::cout << a << std::endl;
}

int main(int argc,char** argv)
{
    runtime = new Runtime();
    sched_sp = new char[sizeof(void*)];

    // 制造任务
    int* fi = new int;
    *fi = 100;
    go(foo,fi);

    char* name=(char*)"liu";
    go(hi,name);

    runtime->schedule();

    return 0;
}