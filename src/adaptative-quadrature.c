#include <adaptative-quadrature.h>

int _acceptable_approx(adaptavive_quadrature_args* args, double* total);
adaptavive_quadrature_intervals _get_intervals(adaptavive_quadrature_args* args, int num_intervals);
void* _call(void* arg);

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

void* pthread_adaptavive_quadrature(void* arg, int num_intervals)
{
    adaptavive_quadrature_args* args = (adaptavive_quadrature_args*) arg;
    double* total = (double*) malloc(sizeof(double));
    *total = 0;

    adaptavive_quadrature_intervals intervals = _get_intervals(args, num_intervals);

    pthread_t* thds = (pthread_t*) malloc(intervals.divisions * sizeof(pthread_t));
    for (int i = 0; i < intervals.divisions; i++)
    {
        pthread_create(&(thds[i]), NULL, _call, &(intervals.args[i]));
    }
    void* res;
    for (int i = 0; i < intervals.divisions; i++)
    {
        pthread_join(thds[i], &res);
        *total += *(double*)res;
    }
    return total;        
}

void* omp_adaptavive_quadrature(adaptavive_quadrature_args* args, int num_intervals)
{
    double* total = (double*) malloc(sizeof(double));
    *total = 0;

    adaptavive_quadrature_intervals intervals = _get_intervals(args, num_intervals);
    double res;
    #pragma omp parallel
    {
        #pragma omp for private(res)
        for (int i = 0; i < intervals.divisions; i++)
        {
            res = *(adaptavive_quadrature(&(intervals.args[i])));
            #pragma omp critical (Total)
            {
                *total += res;
            }
        }    
    }
    return total;
}

void omp_adaptavive_quadrature_admin(
    double* total,
    adaptavive_quadrature_args* initial,
    Queue_v* queue,
    int num_intervals
)
{
    adaptavive_quadrature_intervals intervals = _get_intervals(initial, num_intervals);
    for (int i = 0; i < intervals.divisions; i++)
    {
        enqueue(queue, &(intervals.args[i]));
    }
    
    int empty = 0;
    #pragma omp parallel
    {
        #pragma omp for firstprivate(empty)
        for (int i = 0; i < omp_get_num_threads(); i++)
        {
            // printf("Thread %d executa a iteração %d do loop\n", omp_get_thread_num(), i);
            while(!empty)
            {
                adaptavive_quadrature_args* initial;
                #pragma omp critical
                {
                    initial = !empty ? (adaptavive_quadrature_args*) dequeue(queue) : NULL;
                }
                if(initial != NULL)
                    omp_adaptavive_quadrature_worker(total, initial, queue);
                empty = isEmpty(queue);
            }
        }
    }
    
}

void omp_adaptavive_quadrature_worker(
    double* total,
    adaptavive_quadrature_args* initial,
    Queue_v* queue
)
{
    adaptavive_quadrature_args* args = initial;  
    double tarea = 0;
    double m;
    int approx;
    
    m = (args->r + args->l)/2;
    approx = _acceptable_approx(args, &tarea);
    

    if(!approx)
    {        
        adaptavive_quadrature_args* largs = create_adaptative_quadrature_args(args->l, m, args->func, args->approx);
        adaptavive_quadrature_args* rargs = create_adaptative_quadrature_args(m, args->r, args->func, args->approx);
        #pragma omp critical
        {
            enqueue(queue, largs);
            enqueue(queue, rargs);
        }
    } else {
        *total += tarea; 
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

adaptavive_quadrature_intervals _get_intervals(adaptavive_quadrature_args* args, int num_intervals)
{
    adaptavive_quadrature_intervals intervals;
    if(num_intervals <= 1)
    {
        intervals.args = (adaptavive_quadrature_args*) malloc(sizeof(adaptavive_quadrature_args));
        intervals.divisions = 1;
        intervals.args[0] = *args;
    } else {
        double step = (args->r - args->l) / num_intervals;
        int div = (args->r - args->l) / num_intervals;
        int rem =  (int)(args->r - args->l) >= num_intervals ? (int)(args->r - args->l) % num_intervals : 1;
        intervals.divisions = (num_intervals + !!rem);
        intervals.args = (adaptavive_quadrature_args*) malloc(intervals.divisions * sizeof(adaptavive_quadrature_args));

        //greedy
        double it = args->l;
        for (int i = 0; i < num_intervals; i++)
        {
            intervals.args[i].l = it;
            intervals.args[i].r = it + step;
            intervals.args[i].func = args->func;
            intervals.args[i].approx = args->approx;
            it = intervals.args[i].r;
        }
        if(rem != 0)
        {
            intervals.args[intervals.divisions - 1].l = it;
            intervals.args[intervals.divisions - 1].r = args->r;
            intervals.args[intervals.divisions - 1].func = args->func;
            intervals.args[intervals.divisions - 1].approx = args->approx;
        }
    }
    
    return intervals;
}

void* _call(void* arg)
{
    adaptavive_quadrature_args* args = (adaptavive_quadrature_args*) arg;
    return adaptavive_quadrature(args);
}