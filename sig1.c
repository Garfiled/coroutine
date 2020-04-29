/* Example of using sigaction() to setup a signal handler with 3 arguments
 * including siginfo_t.
 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>


struct stackt {
	char*	ss_sp;
	uint64_t	ss_size;
	int32_t	ss_flags;
	char	pad_cgo_0[4];
};

struct ucontext {
	int32_t uc_onstack;
	uint32_t uc_sigmask;
	struct stackt uc_stack;
	struct ucontext* uc_link;    
	uint64_t uc_mcsize;
	struct mcontext64* uc_mcontext; 
};

struct exceptionstate64 {
	uint16_t trapno;
	uint16_t cpu;
	uint32_t err;
	uint64_t faultvaddr;
};



struct regs64 {
	uint64_t  	rax;
	uint64_t	rbx;
	uint64_t	rcx;
	uint64_t	rdx;
	uint64_t	rdi;
	uint64_t	rsi;
	uint64_t	rbp;
	uint64_t	rsp;
	uint64_t	r8;
	uint64_t	r9;
	uint64_t	r10;
	uint64_t	r11;
	uint64_t	r12;
	uint64_t 	r13;
	uint64_t	r14;
	uint64_t	r15;
	uint64_t	rip;
	uint64_t	rflags;
	uint64_t	cs;
	uint64_t	fs;
	uint64_t	gs;
};



struct mcontext64 {
	struct exceptionstate64 es; 
	struct regs64 ss;
	// fs        floatstate64
	// pad_cgo_0 [4]byte
};

void hi() 
{
	printf("hi\n");
}
 
static void hdl (int sig, siginfo_t *siginfo, void* context)
{
	printf ("Sending PID: %ld, UID: %ld\n",
			(long)siginfo->si_pid, (long)siginfo->si_uid);

	struct ucontext* uctx = (struct ucontext*)context;

	struct regs64* reg = &(uctx->uc_mcontext->ss);

	printf("%llu\n", reg->rip);

	uint64_t pc = reg->rip;
	uint64_t sp = reg->rsp;

	sp -= sizeof(uint64_t);

	*((uint64_t*)sp) = pc;

	reg->rsp = sp;
	reg->rip = hi;
}
 
int main (int argc, char *argv[])
{
	struct sigaction act;
 
	memset (&act, '\0', sizeof(act));
 
	/* Use the sa_sigaction field because the handles has two additional parameters */
	act.sa_sigaction = &hdl;
 
	/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	act.sa_flags = SA_ONSTACK | SA_SIGINFO ;
 
	if (sigaction(SIGINT, &act, NULL) < 0) {
		perror ("sigaction");
		return 1;
	}
 
	while (1)
	{}
	
 
	return 0;
}
