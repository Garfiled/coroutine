# Coroutine
implement c++ coroutine

# Use
```
// create coroutine job
int* fi = new int;
*fi = 100;
go(foo,fi);

char* name=(char*)"liu";
go(hi,name);

go(loop,NULL);
```

# Features
- simple use with go keyword
- support yield to schedule
- support signal to auto schedule

# Todo
- integrate with net epoll
- support 64bit
- signal will terminate sleep/read...syscall (how to fix?)
