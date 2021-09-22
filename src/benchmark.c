#include <benchmark.h>

long int mstopwatch(void (*routine)(void*), void* args)
{
    struct timeval current_time;
    
    gettimeofday(&current_time, NULL);
    double tic = (double)current_time.tv_sec + current_time.tv_usec / 1000000.0;
    
    routine(args);

    gettimeofday(&current_time, NULL);
    double toc = (double)current_time.tv_sec + current_time.tv_usec / 1000000.0;

    return (long int)((toc - tic) * 1000);
}

long int ustopwatch(void (*routine)(void*), void* args)
{
        struct timeval current_time;
    
    gettimeofday(&current_time, NULL);
    double tic = (double)current_time.tv_sec + current_time.tv_usec / 1000000.0;
    
    routine(args);

    gettimeofday(&current_time, NULL);
    double toc = (double)current_time.tv_sec + current_time.tv_usec / 1000000.0;

    return (long int)((toc - tic) * 1000000);
}