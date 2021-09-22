#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


double abs_sinc(double x)
{
    double res = x != 0 ? fabs(sin(x) / (x)) : 1.0;
    return res;
}

double carga(double x)
{

    double power[8] = {5731, 1243, 774, 736, 460, 50, 87, 126};
    double frequencies[8] = {0.11, 0.2, 0.53, 0.74, 1.1, 1.53, 2.3, 2};
    double phases[8] = {0.36, 3.5, 1.5, 1.9, -2.5, -1.36, -5, -1.2};

    double sum = 0;
    for (int i = 0; i < 8; i++)
    {
        /* code */
        sum += power[i] * sin(frequencies[i] * x + phases[i]);
    }
    return fabs(sum);
}

double offsetSin(double x)
{
    double y1 = sin((1.0/2.0) * M_PI * x);
    double y2 = cos((1.0/5.0) * M_PI * x);
    double y3 = sin((1.0/10.0) * M_PI * x);
    return y1 + y2 + y3 + 5;
}

typedef struct adaptavive_quadrature_args
{
    double l;
    double r;
    double (*func)(double);
    double approx;
} adaptavive_quadrature_args;

#ifdef _OPENMP
#include <omp.h>

void delay(int max_delay)
{
    sleep(rand() % max_delay);
}


void* omp_adaptavive_quadrature_thread(adaptavive_quadrature_args* args)
{
    double* total = (double*) malloc(sizeof(double));
    double tarea, larea, rarea, m;

    double l = args->l;
    double r = args->r;
    double fl = args->func(args->l);
    double fr = args->func(args->r);

    m = (args->r + args->l)/2;
    double fm = args->func(m);
    
    //calculate leftlr
    larea = (fl + fm)*(-l + m) / 2;
    //calculate right
    rarea = (fm + fr)*(-m + r) / 2;
    //calculate total
    tarea = (fl + fr)*(-l + r) / 2;
    
    if(/** Acceptable Aprox **/ fabs(tarea - (larea + rarea)) <= args->approx)
    {
        //result
        *total = tarea;
        return total;
    }
    else
    {

        void* lresult;
        void* rresult;
        adaptavive_quadrature_args largs = {args->l, m, args->func, args->approx};
        adaptavive_quadrature_args rargs = {m, args->r, args->func, args->approx};
        
        #pragma omp parallel
        {
            #pragma omp sections
            {
                #pragma omp section
                {
                    lresult = omp_adaptavive_quadrature_thread(&largs);
                }
                #pragma omp section
                {
                    rresult = omp_adaptavive_quadrature_thread(&rargs);
                }
            }
        }
        
        *total = *((double*)lresult) + *((double*)rresult);

        return total;
    }
}

void* adaptavive_quadrature_thread(void* arg)
{
    adaptavive_quadrature_args* args = (adaptavive_quadrature_args*) arg;
    double* total = (double*) malloc(sizeof(double));
    double tarea, larea, rarea, m;

    double l = args->l;
    double r = args->r;
    double fl = args->func(args->l);
    double fr = args->func(args->r);

    m = (args->r + args->l)/2;
    double fm = args->func(m);
    
    //calculate leftlr
    larea = (fl + fm)*(-l + m) / 2;
    //calculate right
    rarea = (fm + fr)*(-m + r) / 2;
    //calculate total
    tarea = (fl + fr)*(-l + r) / 2;

    if(/** Acceptable Aprox **/ fabs(tarea - (larea + rarea)) <= args->approx)
    {
        //result
        *total = tarea;
        return total;
    }
    else {
        
        //left
        pthread_t lthread;
        adaptavive_quadrature_args largs = {args->l, m, args->func, args->approx};
        pthread_create(&lthread, NULL, adaptavive_quadrature_thread, &largs);
        void* lresult;

        //right
        pthread_t rthread;
        adaptavive_quadrature_args rargs = {m, args->r, args->func, args->approx};
        pthread_create(&rthread, NULL, adaptavive_quadrature_thread, &rargs);
        void* rresult;
        
        pthread_join(lthread, &lresult);
        pthread_join(rthread, &rresult);
        
        *total = *((double*)lresult) + *((double*)rresult);
        return total;        
    }
}

    
int main(int argc, char *argv[])
{
    omp_set_num_threads(4);

    struct timeval current_time;
    double total = 0;

    //Argument Input
    double L = (double) strtof(argv[1], NULL);
    double R = (double) strtof(argv[2], NULL);
    double A = (double) strtof(argv[3], NULL);
    printf("Parametros l, r, aproximation = %.1f %.1f %.9f\n", L, R, A);

    adaptavive_quadrature_args args = {L, R, abs_sinc, A};
    // adaptavive_quadrature_args args = {-10, 10, abs_sinc, 0.00001};

    // Pthread
    printf("START PHTREAD********************\n");
    gettimeofday(&current_time, NULL);
    double tic = (double)current_time.tv_sec + current_time.tv_usec / 1000000.0;

    pthread_t thread;
    pthread_create(&thread, NULL, adaptavive_quadrature_thread, &args);
    
    void* res;
    pthread_join(thread, &res);
    total = *((double*) res);
    
    printf("total: %2f\n", total);

    gettimeofday(&current_time, NULL);
    double toc = (double)current_time.tv_sec + current_time.tv_usec / 1000000.0;

    printf("******************** END PTHREAD\n");
    printf("Elapsed: %.0f miliseconds\n\n", (double)(toc - tic) * 1000);
 
    //OpenMP
    printf("START OPENMP********************\n");
    gettimeofday(&current_time, NULL);
    tic = (double)current_time.tv_sec + current_time.tv_usec / 1000000.0;

    #pragma omp parallel
    {
        total = *((double*) omp_adaptavive_quadrature_thread(&args));
    }
    printf("total: %2f\n", total);

    gettimeofday(&current_time, NULL);
    toc = (double)current_time.tv_sec + current_time.tv_usec / 1000000.0;

    printf("******************** END OPENMP\n");
    printf("Elapsed: %.0f miliseconds\n\n", (double)(toc - tic) * 1000);

    return 0;
}
#endif


