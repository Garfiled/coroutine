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
```

# Features
- simple use with go keyword
- yield to schedule

# Todo
- integrate with net epoll
- support 64bit
