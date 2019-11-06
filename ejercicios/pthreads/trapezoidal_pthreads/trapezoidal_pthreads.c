#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

typedef struct
{
    double area;
    double lower_bound;
    double upper_bound;
    double division_quantity;
    size_t thread_count;
    double jump;
    pthread_mutex_t mutex;
}shared_data_t;

typedef struct
{
    shared_data_t* shared;
    double local_area;
    size_t thread_num;
}private_data_t;

int create_thread(shared_data_t* shared);
void* run(void* data);
double positive_function(double preimage);
double trapezoidal_area(double (*function)(double), double lower_bound, double upper_bound, double division_quantity);
double maping(size_t thread_num, double divisions, double thread_count);

int main (int argc, char* argv[])
{
    shared_data_t* shared = (shared_data_t*) calloc(1, sizeof (shared_data_t));
    if(shared == NULL)
           return (void)fprintf(stderr,"error, couldn't allocate shared memory\n"), 1;
    shared->thread_count = (size_t)(sysconf(_SC_NPROCESSORS_ONLN));

    if(argc < 4){
        free(shared);
        return (void)fprintf(stderr,"not enough arguments\n"), 1;
    }

    if ((double) atof(argv[1]) < 0){
        free(shared);
        return (void)fprintf(stderr,"The lower bound can't be a negative number\n"), 1;
    }

    if ((double) atof(argv[1]) > (atof)(argv[2])){
        free(shared);
        return (void)fprintf(stderr,"The lower bound can't be greater than the upper bound\n"), 1;
    }

    if ((double)atof(argv[3]) == 0.0){
        free(shared);
        return (void)fprintf(stderr,"The amount of division can't be  0\n"), 1;
    }
    
    if(argc > 4){
		shared->thread_count = strtoull(argv[4], NULL, 10);	
	}

    shared->lower_bound = (double) atof(argv[1]);
    shared->upper_bound = (double) atof(argv[2]);
    shared->division_quantity = (double) atof(argv[3]);
    shared->jump = (shared->upper_bound - shared->lower_bound)/shared->division_quantity;

    //Inicia el coteo del tiempo de creación
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    //Se crean los threads
    int error = create_thread(shared);
    if (error)
        return error;

    //Se determina el tiempo de creación
    struct timespec finish_time;
    clock_gettime(CLOCK_MONOTONIC, &finish_time);
    ///printf("Hello world from main thread\n");

    double elapsed_seconds = finish_time.tv_sec - start_time.tv_sec
        + 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);

    printf("Execution time %.9lfs\n", elapsed_seconds);
	
	shared->area *= shared->jump/2;
    printf("the area is %f \n", shared->area);

    free(shared);
    return 0;
}

/**
 * Funcion: Se crean todos los threads y se destruyen
 * Param: Se recibe un putero que apunta a los datos compartidos por los threads.
 * Return: Nada
 **/
int create_thread(shared_data_t* shared)
{
    //Se asigna memoria dinámica para tener el array de threads
    pthread_t* threads = (pthread_t*) malloc(shared->thread_count * sizeof(pthread_t));
    if (threads == NULL)
        return (void)fprintf(stderr, "error: could not allocate memory for %zu threads\n", shared->thread_count), 1;

    //Se crea el private data para cada thread
    private_data_t* private = (private_data_t*) calloc(shared->thread_count, sizeof(private_data_t));
    if(private == NULL)
        return (void)fprintf(stderr, "error: could not allocate private memory for %zu threads\n", shared->thread_count), 1;

    //Se llenan los datos de cada private data y se crea el thread
    for(size_t index = 0; index < shared->thread_count; ++index)
    {
        private[index].thread_num = index;
        private[index].shared = shared;
        pthread_create(&threads[index], NULL, run, &private[index]);
    }

    //El programa se espera a que todos los threads terminen
    for(size_t index = 0; index < shared->thread_count; ++index)
        pthread_join(threads[index], NULL);

    free(private);
    free(threads);

    return 0;
}

/**
 * Funcion: Función que ejecuta cada thread, en este caso imprimir hola mundo
 * Param: Se recibe un putero con el private data de cada thread
 * Return: Nada
 **/
void* run(void* data)
{
    private_data_t* private = (private_data_t *) data;
    shared_data_t* shared = private->shared;
    //private->local_area = trapezoidal_area(positive_function, shared->lower_bound, shared->upper_bound, shared->division_quantity);

    double local_lower_bound = private->thread_num*shared->jump;
	double local_area = 0;
	
	while(local_lower_bound < shared->upper_bound){
		local_area += (positive_function(local_lower_bound) + positive_function(local_lower_bound + shared->jump));
		local_lower_bound += shared->jump * shared->thread_count;
	}
	
    //private->local_area = trapezoidal_area(positive_function, local_lower_bound, local_upper_bound, shared->division_quantity);

    pthread_mutex_lock(&shared->mutex);
    shared->area += local_area;
    pthread_mutex_unlock(&shared->mutex);
    return NULL;
}

double positive_function(double x){
    double image = sqrt((x*(pow(x,2) - 6*x + 9))/9);
    return image;
}

/*
double trapezoidal_area(double (*function)(double), double lower_bound, double upper_bound, double division_quantity){
    double area = 0;
    double jump = ((upper_bound-lower_bound)/division_quantity);
    while (lower_bound <  upper_bound) {
        area += (function(lower_bound) + function(lower_bound+jump));
        lower_bound += jump;
    }
    area *= (jump/2);
    return area;
}

double maping(size_t thread_num, double divisions, double thread_count){
    return thread_num * floor(divisions/thread_count) + fmin(thread_num, fmod(divisions, thread_count));
}
*/
