#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

typedef struct
{
	size_t thread_counter; //contador de threads
    sem_t* semaphores; //array de semaforo
}shared_data_t;

typedef struct
{
	size_t thread_num; //ID del thread
	shared_data_t* shared_data;
}private_data_t;

int create_thread(shared_data_t* shared_data);
void* run(void* data);

int main (int argc, char* argv[])
{
	//se asigna el shared data a memoria dinámica
	shared_data_t* shared_data = (shared_data_t*) calloc(1, sizeof(shared_data_t)); 
	if (shared_data == NULL)
		return (void)fprintf(stderr, "error: could not allocate shared memory\n"), 1;
    #include <semaphore.h>

	//cantidad de threads por default
    shared_data->thread_counter = (size_t)(sysconf(_SC_NPROCESSORS_ONLN));
    #include <semaphore.h>

	//cantidad de threads si se recibe parámetro
	if(argc >= 2)
		shared_data->thread_counter = strtoull(argv[1], NULL, 10);	
	
    //se asigna el espacio de memoria para el array de semaforos
    shared_data->semaphores = (sem_t*) malloc(shared_data->thread_counter * sizeof(sem_t));
	
    //Se crean todos los mutex, el primero se crea por separado con un valor de 1, todos los demás se crean con 0
    sem_init(&shared_data->semaphores[0], 0, 1);
    for(size_t index = 1; index < shared_data->thread_counter; ++index)
        sem_init(&shared_data->semaphores[index], 0, 0);
	
    //Inicia el conteo del tiempo de creación
	struct timespec start_time;
	clock_gettime(CLOCK_MONOTONIC, &start_time);
	
	//Se crean los threads
	int error = create_thread(shared_data);
	if (error)
		return error;
	
	//Se determina el tiempo de creación	
	struct timespec finish_time;
	clock_gettime(CLOCK_MONOTONIC, &finish_time);
	///printf("Hello world from main thread\n");
	
	double elapsed_seconds = finish_time.tv_sec - start_time.tv_sec
		+ 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);
	
	printf("Hello execution time %.9lfs\n", elapsed_seconds);
	free(shared_data);
	return 0;	
}

/**
 * Funcion: Se crean todos los threads y se destruyen
 * Param: Se recibe un putero que apunta a los datos compartidos por los threads.
 * Return: Nada
 **/
int create_thread(shared_data_t* shared_data)
{
	//Se asigna memoria dinámica para tener el array de threads
	pthread_t* threads = (pthread_t*) malloc(shared_data->thread_counter * sizeof(pthread_t));
	if (threads == NULL)
		return (void)fprintf(stderr, "error: could not allocate memory for %zu threads\n", shared_data->thread_counter), 1;
	
	//Se crea el private data para cada thread
	private_data_t* private_data = (private_data_t*) calloc(shared_data->thread_counter, sizeof(private_data_t));
	if(private_data == NULL)
		return (void)fprintf(stderr, "error: could not allocate private memory for %zu threads\n", shared_data->thread_counter), 1;
	
	//Se llenan los datos de cada private data y se crea el thread
	for(size_t index = 0; index < shared_data->thread_counter; ++index)
	{
		private_data[index].thread_num = index;
		private_data[index].shared_data = shared_data;
		pthread_create(&threads[index], NULL, run, &private_data[index]);
	}
	
	//El programa se espera a que todos los threads terminen
	for(size_t index = 0; index < shared_data->thread_counter; ++index)
		pthread_join(threads[index], NULL);
	
	free(private_data);
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
	private_data_t* private_data = (private_data_t *) data;
	shared_data_t* shared_data = private_data->shared_data;
	size_t thread_num = private_data->thread_num;
	size_t thread_counter = shared_data->thread_counter;
	
    //La zona crítica se controla con el semaforo correspondiente y se espera a que habiliten el semaforo en caso de que este bloqueado
    sem_wait(&shared_data->semaphores[thread_num]);
	printf("Hello world from secondary thread %zu of %zu\n", thread_num, thread_counter);
	
	//Revisa si todavía hay un mutex que abrir
	if(thread_num < thread_counter-1)
	{
        sem_post(&shared_data->semaphores[thread_num+1]);
	}
    sem_post(&shared_data->semaphores[thread_num]);
	return NULL;
}
