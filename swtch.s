.extern sched_sp
.extern curr_co

.global corun
.global yield

corun:
	pushl %ebp
    pushl %ebx
    pushl %esi
    pushl %edi

	movl sched_sp,%edx
	movl %esp,(%edx)

	movl curr_co,%edx
    movl 24(%edx),%esp

	popl %edi
  	popl %esi
  	popl %ebx
  	popl %ebp

	movl 28(%edx),%eax
	movl $0,%ecx
	#此处cmpl好像不能跟立即数
	cmpl %eax,%ecx
	jne  resume

	movl 20(%edx),%eax
	call *%eax

	movl sched_sp,%edx
	movl (%edx),%esp

  	popl %edi
   	popl %esi
   	popl %ebx
   	popl %ebp
	# 标识是函数正常运行完返回
	movl $0,%eax
resume:
	ret

yield:
	pushl %ebp
  	pushl %ebx
  	pushl %esi
  	pushl %edi

	# save esp
	movl curr_co,%edx
	movl %esp,24(%edx)

	# swtch stack
	movl sched_sp,%edx
	movl (%edx),%esp

	popl %edi
    popl %esi
    popl %ebx
    popl %ebp

	// 标识是yield返回
	movl $1,%eax
	ret