#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include <queue.h>
#include <utils.h>
#include <benchmark.h>
#include <adaptative-quadrature.h>

#ifdef _OPENMP
#include <omp.h>
#endif

void omp_routine(void* args)
{
    adaptavive_quadrature_args* qargs = (adaptavive_quadrature_args*) args;
    double total = 0;
    Queue_v* queue = createQueue(8);
    sem_t mutex;
    sem_init(&mutex, 0, 1);

    omp_adaptavive_quadrature_admin(&total, qargs, queue, &mutex);
    printf("total: %2f\n", total);
}

int main(int argc, char *argv[])
{
    // omp_set_num_threads(8);

    // //Argument Input
    // double L = (double) strtof(argv[1], NULL);
    // double R = (double) strtof(argv[2], NULL);
    // double A = (double) strtof(argv[3], NULL);
    // printf("Parametros l, r, aproximation = %.1f %.1f %.9f\n", L, R, A);

    // adaptavive_quadrature_args args = {L, R, abs_sinc, A};
    adaptavive_quadrature_args args = {-10, 10, abs_sinc, 0.00001};


    //OpenMP
    printf("START OPENMP********************\n");

    long int omp_time = (long int) ustopwatch(omp_routine, &args);

    printf("******************** END OPENMP\n");
    printf("Elapsed: %ld microseconds\n\n", omp_time);

    return 0;
}