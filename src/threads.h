typedef int (*thread_fn)(void*);

int createThread(thread_fn fn, void* arg);