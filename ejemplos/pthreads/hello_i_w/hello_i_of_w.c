#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct
{
		size_t thread_counter;
}shared_data_t;

typedef struct
{
	size_t thread_num;
	shared_data_t* shared_data;
}private_data_t;

void* run(void* data)
{
	private_data_t* private_data = (private_data_t *) data;
	shared_data_t* shared_data = private_data->shared_data;
	size_t thread_num = private_data->thread_num;
	size_t thread_counter = shared_data->thread_counter; 
	printf("Hello world from secondary thread %zu\n of %zu\n", thread_num, thread_counter);
	return NULL;
}

int main (int argc, char* argv[])
{
	size_t thread_counter = sysconf(_SC_NPROCESSORS_ONLN);
	
	if(argc >= 2)
		thread_counter = strtoull(argv[1], NULL, 10);	
	
	pthread_t* threads = (pthread_t*) malloc(thread_counter * sizeof(pthread_t));
	if (threads == NULL)
		return (void)fprintf(stderr, "error: could not allocate memory for %zu threads\n", thread_counter), 1;
	
	shared_data_t* shared_data = (shared_data_t*) calloc(1, sizeof(shared_data_t));
	if (shared_data == NULL)
		return (void)fprintf(stderr, "error: could not allocate shared memory for %zu threads\n", thread_counter), 1;
	
	shared_data->thread_counter = thread_counter;
	
	private_data_t* private_data = (private_data_t*) calloc(thread_counter, sizeof(private_data_t));
	if(private_data == NULL)
		return (void)fprintf(stderr, "error: could not allocate private memory for %zu threads\n", thread_counter), 1;
	
	for(size_t index = 0; index < thread_counter; ++index)
	{
		private_data[index].thread_num = index;
		private_data[index].shared_data = shared_data;
		pthread_create(&threads[index], NULL, run, &private_data[index]);
	}
	
	for(size_t index = 0; index < thread_counter; ++index)
		pthread_join(threads[index], NULL);
	
	printf("Hello world from main thread\n");
	
	free(private_data);
	free(shared_data); 
	free(threads);
	
	return 0;
}
