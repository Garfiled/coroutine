struct local_stackt {
    char*   ss_sp;
    int32_t ss_flags;
    uint32_t    ss_size;
};

struct local_sigcontext {
    uint16_t gs;
    uint16_t __gsh;
    uint16_t fs;
    uint16_t __fsh;
    uint16_t es;
    uint16_t __esh;
    uint16_t ds;
    uint16_t __dsh;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t trapno;
    uint32_t err;
    uint32_t eip;
    uint16_t cs;
    uint16_t __csh;
    uint32_t eflags;
    uint32_t esp_at_signal;
    uint16_t ss;
    uint16_t __ssh;
    void* fpstate;
    uint32_t oldmask;
    uint32_t cr2;
};


struct local_ucontext {
    uint32_t uc_flags;
    struct ucontext* uc_link;    
    struct local_stackt uc_stack;
    struct local_sigcontext uc_mcontext; 
    uint32_t uc_sigmask;
};
