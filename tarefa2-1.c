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

void sequential_routine(void* args)
{
    double* res = adaptavive_quadrature(args);
    double total = *(res);
    
    printf("total: %2f\n", total);
}

void pthread_routine(void* args)
{
    pthread_t thread;
    pthread_create(&thread, NULL, pthread_adaptavive_quadrature, args);
    
    void* res;
    pthread_join(thread, &res);
    double total = *((double*) res);
    
    printf("total: %2f\n", total);
}

void omp_routine(void* args)
{
    double total = 0;
    #pragma omp parallel
    {
        total = *((double*) omp_adaptavive_quadrature(args));
    }
    printf("total: %2f\n", total);
}

int main(int argc, char *argv[])
{
    omp_set_num_threads(8);

    //Argument Input
    double L = (double) strtof(argv[1], NULL);
    double R = (double) strtof(argv[2], NULL);
    double A = (double) strtof(argv[3], NULL);
    printf("Parametros l, r, aproximation = %.1f %.1f %.9f\n", L, R, A);

    adaptavive_quadrature_args args = {L, R, abs_sinc, A};
    // adaptavive_quadrature_args args = {-10, 10, abs_sinc, 0.00001};

    // Pthread
    printf("START SEQUENTIAL********************\n");

    long int sequential_time = (long int) ustopwatch(sequential_routine, &args);

    printf("******************** END SEQUENTIAL\n");
    printf("Elapsed: %ld microseconds\n\n", sequential_time);

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


