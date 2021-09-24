#ifndef ADAPTATIVE_QUADRATURE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include <queue.h>

#ifdef _OPENMP
#include <omp.h>
#endif

typedef struct adaptavive_quadrature_args
{
    double l;
    double r;
    double (*func)(double);
    double approx;
} adaptavive_quadrature_args;

double* adaptavive_quadrature(adaptavive_quadrature_args* args);
void* pthread_adaptavive_quadrature(void* arg);
void* omp_adaptavive_quadrature(adaptavive_quadrature_args* args);
void omp_adaptavive_quadrature_admin(double* total, adaptavive_quadrature_args* initial , Queue_v* queue, sem_t* mutex);
void omp_adaptavive_quadrature_worker(double* total, Queue_v* queue, sem_t* mutex);


#endif // !ADAPTATIVE_QUADRATURE_H