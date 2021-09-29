#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include <utils.h>
#include <benchmark.h>
#include <adaptative-quadrature.h>

#ifdef _OPENMP
#include <omp.h>

typedef struct _r_args {adaptavive_quadrature_args aq_args; int num_intervals;} _r_args;

void sequential_routine(void* args)
{
    double* res = adaptavive_quadrature(args);
    double total = *(res);
    
    printf("total: %2f\n", total);
}

void pthread_routine(void* arg)
{
    _r_args* args = (_r_args*) arg;
    void* res;
    res = pthread_adaptavive_quadrature(&(args->aq_args), args->num_intervals);
    double total = *((double*) res);
    
    printf("total: %2f\n", total);
}

void omp_routine(void* arg)
{
    _r_args* args = (_r_args*) arg;
    double total = 0;
    #pragma omp parallel
    {
        #pragma omp single
        {
            total = *((double*) omp_adaptavive_quadrature(&(args->aq_args), args->num_intervals));
        }
    }
    printf("total: %2f\n", total);
}

int main(int argc, char *argv[])
{
    omp_set_num_threads(8);
    omp_set_nested(1);

    //Argument Input
    double L = (double) strtof(argv[1], NULL);
    double R = (double) strtof(argv[2], NULL);
    double A = (double) strtof(argv[3], NULL);
    int NI = (int) strtol(argv[4], NULL , 10);
    printf("Parametros l, r, aproximation, number f intervals = %.1f %.1f %.9f %d\n", L, R, A, NI);

    adaptavive_quadrature_args aq_args = {L, R, abs_sinc, A};
    _r_args args = {aq_args, NI};
    // adaptavive_quadrature_args aq_args = {0, 10, abs_sinc, 0.00001};
    // _r_args args = {aq_args, 4};

    // // Sequential
    // printf("START SEQUENTIAL********************\n");

    // long int sequential_time = (long int) ustopwatch(sequential_routine, &args);

    // printf("******************** END SEQUENTIAL\n");
    // printf("Elapsed: %ld microseconds\n\n", sequential_time);

    // Pthread
    printf("START PHTREAD********************\n");

    long int pthread_time = (long int) ustopwatch(pthread_routine, &args);

    printf("******************** END PTHREAD\n");
    printf("Elapsed: %ld microseconds\n\n", pthread_time);
 
    //OpenMP
    printf("START OPENMP********************\n");

    long int omp_time = (long int) ustopwatch(omp_routine, &args);

    printf("******************** END OPENMP\n");
    printf("Elapsed: %ld microseconds\n\n", omp_time);

    return 0;
}
#endif


