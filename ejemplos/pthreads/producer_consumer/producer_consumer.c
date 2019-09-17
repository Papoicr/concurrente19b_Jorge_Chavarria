#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

typedef struct
{
	size_t thread_counter;
	size_t position;
	pthread_mutex_t position_mutex;
}shared_data_t;

typedef struct
{
	size_t thread_num;
	shared_data_t* shared_data;
}private_data_t;

int create_thread(shared_data_t* shared_data);
void* run(void* data);

int main (int argc, char* argv[])
{
	shared_data_t* shared_data = (shared_data_t*) calloc(1, sizeof(shared_data_t));
	if (shared_data == NULL)
		return (void)fprintf(stderr, "error: could not allocate shared memory\n"), 1;
	
	shared_data->thread_counter = sysconf(_SC_NPROCESSORS_ONLN);
	pthread_mutex_init(&shared_data->position_mutex, /*attr*/ NULL);
	shared_data->position = 0;
	
	if(argc >= 2)
		shared_data->thread_counter = strtoull(argv[1], NULL, 10);	
	
	struct timespec start_time;
	clock_gettime(CLOCK_MONOTONIC, &start_time);
	
	int error = create_thread(shared_data);
	if (error)
		return error;
		
	struct timespec finish_time;
	clock_gettime(CLOCK_MONOTONIC, &finish_time);

	double elapsed_seconds = finish_time.tv_sec - start_time.tv_sec
		+ 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);
	
	printf("Hello execution time %.9lfs\n", elapsed_seconds);
	
	pthread_mutex_destroy(&shared_data->position_mutex);
	free(shared_data);
	return 0;	
}

int create_thread(shared_data_t* shared_data)
{
	pthread_t* threads = (pthread_t*) malloc(shared_data->thread_counter * sizeof(pthread_t));
	if (threads == NULL)
		return (void)fprintf(stderr, "error: could not allocate memory for %zu threads\n", shared_data->thread_counter), 1;
	
	private_data_t* private_data = (private_data_t*) calloc(shared_data->thread_counter, sizeof(private_data_t));
	if(private_data == NULL)
		return (void)fprintf(stderr, "error: could not allocate private memory for %zu threads\n", shared_data->thread_counter), 1;
	
///Inicia la concurrencia a partir de aquí
	for(size_t index = 0; index < shared_data->thread_counter; ++index)
	{
		private_data[index].thread_num = index;
		private_data[index].shared_data = shared_data;
		pthread_create(&threads[index], NULL, run, &private_data[index]);
	}
	
	pthread_mutex_lock(&shared_data->position_mutex);
	printf("Hello world from main thread\n");
	pthread_mutex_unlock(&shared_data->position_mutex);	
		
	for(size_t index = 0; index < shared_data->thread_counter; ++index)
		pthread_join(threads[index], NULL);
///Termina concurrencia aquí 

	free(private_data);
	free(threads);
	return 0;
}

//Todo este metodo es concurrente
void* run(void* data)
{
	private_data_t* private_data = (private_data_t *) data;
	shared_data_t* shared_data = private_data->shared_data;
	
	size_t thread_num = private_data->thread_num;
	size_t thread_counter = shared_data->thread_counter; ///A pesar de usar el shared data no se genera condicion de carrera por que solo se hace lectura
	
	pthread_mutex_lock(&shared_data->position_mutex);
	
	shared_data->position++;//Hay condición de carrera, por que es concurrente, usa shared data y realiza una escritura (Region critica)
	printf("Thread %zu/%zu: I arrived at position %zu\n", thread_num, thread_counter, shared_data->position);
	
	pthread_mutex_unlock(&shared_data->position_mutex);
	return NULL;
}
/**
 * ----------------------------------------------------------------------
 * Condición de carrera
 * Las condiciones de carrera solo ocurren en concurrencias
 * Solo ocurren en memoria compartida
 * Tiene que existir una modificación (write)
 * ----------------------------------------------------------------------
 * Mecanismo de control de concurrencia = Mecanismo de sincronizacion
 * -Mutex (lock)
 * -Semaphore
 * -Barrier
 * -Condition variable
 * -RW locks
 * ----------------------------------------------------------------------
 * Region critica = lineas de codigo en las que se puede dar una condicion de carrera
 * ---------------------------------------------------------------------- 
 * */
