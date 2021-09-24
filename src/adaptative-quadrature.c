#include <adaptative-quadrature.h>

adaptavive_quadrature_args* create_adaptative_quadrature_args(
    double l,
    double r,
    double (*func)(double),
    double approx
)
{
    adaptavive_quadrature_args* args = (adaptavive_quadrature_args*) malloc(sizeof(adaptavive_quadrature_args));
    args->l = l;
    args->r = r;
    args->func = func;
    args->approx = approx;

    return args;
}

double* adaptavive_quadrature(adaptavive_quadrature_args* args)
{
    double* total = (double*) malloc(sizeof(double));
    double m = (args->r + args->l)/2;
    int approx = _acceptable_approx(args, total);

    if(/** Acceptable Aprox **/ approx)
    {
        //result
        return total;
    }
    else {
        
        //left
        adaptavive_quadrature_args largs = {args->l, m, args->func, args->approx};
        double* lresult = adaptavive_quadrature(&largs);

        //right
        adaptavive_quadrature_args rargs = {m, args->r, args->func, args->approx};
        double* rresult = adaptavive_quadrature(&rargs);
        
        *total = *lresult + *rresult;
        return total;        
    }
}

void* pthread_adaptavive_quadrature(void* arg)
{
    adaptavive_quadrature_args* args = (adaptavive_quadrature_args*) arg;
    double* total = (double*) malloc(sizeof(double));
    double m = (args->r + args->l)/2;
    int approx = _acceptable_approx(args, total);

    if(/** Acceptable Aprox **/ approx)
    {
        //result
        return total;
    }
    else {
        
        //left
        pthread_t lthread;
        adaptavive_quadrature_args largs = {args->l, m, args->func, args->approx};
        pthread_create(&lthread, NULL, pthread_adaptavive_quadrature, &largs);
        void* lresult;

        //right
        pthread_t rthread;
        adaptavive_quadrature_args rargs = {m, args->r, args->func, args->approx};
        pthread_create(&rthread, NULL, pthread_adaptavive_quadrature, &rargs);
        void* rresult;
        
        pthread_join(lthread, &lresult);
        pthread_join(rthread, &rresult);
        
        *total = *((double*)lresult) + *((double*)rresult);
        return total;        
    }
}

void* omp_adaptavive_quadrature(adaptavive_quadrature_args* args)
{
    double* total = (double*) malloc(sizeof(double));
    double m = (args->r + args->l)/2;
    int approx = _acceptable_approx(args, total);

    if(/** Acceptable Aprox **/ approx)
    {
        //result
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
                    lresult = omp_adaptavive_quadrature(&largs);
                }
                #pragma omp section
                {
                    rresult = omp_adaptavive_quadrature(&rargs);
                }
            }
        }
        
        *total = *((double*)lresult) + *((double*)rresult);

        return total;
    }
}

void omp_adaptavive_quadrature_admin(double* total, adaptavive_quadrature_args* initial , Queue_v* queue, sem_t* mutex)
{
    enqueue(queue, initial);
    while(!isEmpty(queue))
    {
        #pragma omp parallel
        {
            #pragma omp for
            for (int i = 0; i < queue->size; i++)
            {
                // printf("Thread %d executa a iteração %d do loop\n", omp_get_thread_num(), i);
                omp_adaptavive_quadrature_worker(total, queue, mutex);
            }
        }
    }
}

void omp_adaptavive_quadrature_worker(double* total, Queue_v* queue, sem_t* mutex)
{
    adaptavive_quadrature_args* args = (adaptavive_quadrature_args*) dequeue(queue);  
    double m = (args->r + args->l)/2;
    double tarea = 0;
    int approx = _acceptable_approx(args, &tarea);
    *total += tarea; 

    if(!approx)
    {        
        adaptavive_quadrature_args* largs = create_adaptative_quadrature_args(args->l, m, args->func, args->approx);
        adaptavive_quadrature_args* rargs = create_adaptative_quadrature_args(m, args->r, args->func, args->approx);
        if(mutex != NULL)
        {
            //wait
            sem_wait(mutex);
            enqueue(queue, largs);
            enqueue(queue, rargs);
            //signal
            sem_post(mutex);  
        } else
        {
            enqueue(queue, largs);
            enqueue(queue, rargs);
        }
    }
}

int _acceptable_approx(adaptavive_quadrature_args* args, double* total)
{
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
    *total = tarea;
     
    return fabs(tarea - (larea + rarea)) <= args->approx;
}