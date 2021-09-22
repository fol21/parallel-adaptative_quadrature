#ifndef ADAPTATIVE_QUADRATURE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#ifdef _OPENMP
#include <omp.h>

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

#endif
#endif // !ADAPTATIVE_QUADRATURE_H