#ifndef BENCHMARK_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <time.h>


long int mstopwatch(void (*routine)(void*), void* args);
long int ustopwatch(void (*routine)(void*), void* args);

#endif // !BENCHMARK_H