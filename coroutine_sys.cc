#include <stdio.h>
#include <queue>
#include <iostream>
#include <signal.h>
#include <string.h>
#include <time.h>

#include "context.h"

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

void loop1(void*)
{
	while (1)
	{
		for (int i=1;i<1000000;i++){
		}
		printf("loop1\n");
	}
}
void loop2(void*)
{
	while (1)
	{
		for (int i=1;i<1000000;i++){
		}
		printf("loop2\n");
	}
}

static void hdl (int sig, siginfo_t *siginfo, void* context) 
{
	
    struct local_ucontext* uctx = (struct local_ucontext*)context;

    struct local_sigcontext* reg = &(uctx->uc_mcontext);

    uint32_t pc = reg->eip;
    uint32_t sp = reg->esp;

    sp -= sizeof(void*);

    *((uint32_t*)sp) = pc;

    reg->esp = sp;
    reg->eip = uint32_t(yield);	
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

    go(loop1,NULL);

    go(loop2,NULL);

    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_sigaction = &hdl;
    act.sa_flags = SA_ONSTACK | SA_SIGINFO |SA_RESTART;
    if (sigaction(SIGUSR1, &act, NULL) < 0) {
       	std::cout << "sigaction err" << std::endl;
        return 1;
    }

	struct sigevent	event;
	timer_t	timer_id;

	event.sigev_value.sival_ptr = &timer_id;
	event.sigev_notify = SIGEV_SIGNAL;
	event.sigev_signo = SIGUSR1;
	timer_create(CLOCK_REALTIME, &event, &timer_id);
	struct itimerspec ts;
	ts.it_interval.tv_sec = 0; // the spacing time
    ts.it_interval.tv_nsec = 10000000;
    ts.it_value.tv_sec = 0;  // the delay time start
    ts.it_value.tv_nsec = 10000000;
	int ret = timer_settime(timer_id, 0, &ts, NULL);
	if (ret)
		printf("set time err:%d",ret);


    runtime->schedule();

    return 0;
}
