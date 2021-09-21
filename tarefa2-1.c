#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <math.h>

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

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
    #pragma omp parallel
    {
            double tarea, larea, rarea, m;
    
            double l = args->l;
            double r = args->r;
            double fl = fabs(args->func(args->l));
            double fr = fabs(args->func(args->r));
    
            m = (args->r + args->l)/2;
            double fm = fabs(args->func(m));
            
            //calculate leftlr
            larea = (fl + fm)*(-l + m) / 2;
            //calculate right
            rarea = (fm + fr)*(-m + r) / 2;
            //calculate total
            
            if(/** Acceptable Aprox **/ tarea - (larea + rarea) <= args->approx)
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
                        #pragma omp section{
                            lresult = omp_adaptavive_quadrature_thread(&largs);
                        }
                        #pragma omp section{
                            rresult = omp_adaptavive_quadrature_thread(&rargs);
                        }
                    }
                }
                
                *total = *((double*)lresult) + *((double*)rresult);
                return total;

            }
    }
}

void* adaptavive_quadrature_thread(void* arg)
{
    adaptavive_quadrature_args* args = (adaptavive_quadrature_args*) arg;
    double* total = (double*) malloc(sizeof(double));
    double tarea, larea, rarea, m;
    
    double l = args->l;
    double r = args->r;
    double fl = fabs(args->func(args->l));
    double fr = fabs(args->func(args->r));
    
    m = (args->r + args->l)/2;
    double fm = fabs(args->func(m));
    
    //calculate leftlr
    larea = (fl + fm)*(-l + m) / 2;
    //calculate right
    rarea = (fm + fr)*(-m + r) / 2;
    //calculate total
    tarea = (fl + fr)*(-l + r) / 2;

    if(/** Acceptable Aprox **/ tarea - (larea + rarea) <= args->approx)
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

    clock_t tic = clock();
    // Argument Input
    double L = (double) strtof(argv[1], NULL);
    double R = (double) strtof(argv[2], NULL);
    double A = (double) strtof(argv[3], NULL);
    printf("Parametros l, r, aproximation = %.1f %.1f %.9f\n", L, R, A);


    // delay(5);
    // #pragma omp parallel
    // {
    //     int i;
    //     #pragma omp for
    //     for (i = 0; i < 8; i++)
    //     {
    //         /* code */
    //         delay(5);
    //         printf(" i Thread %d executada de %d\n", omp_get_thread_num(), omp_get_num_threads());
    //     }
        
    // }

    double total = 0;

    adaptavive_quadrature_args args = {L, R, offsetSin, A};
    pthread_t thread;
    pthread_create(&thread, NULL, adaptavive_quadrature_thread, &args);
    
    void* res;
    pthread_join(thread, &res);
    total = *((double*) res);
    
    printf("total: %2f\n", total);

    clock_t toc = clock();

    printf("Elapsed: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    return 0;
}
#endif


