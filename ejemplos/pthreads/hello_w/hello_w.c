#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void* run(void* data)
{
	//(void)data;
	size_t thread_num = (size_t)data; 
	printf("Hello world from secondary thread %zu\n", thread_num);
	return NULL;
}

int main (int argc, char* argv[])
{
	size_t thread_counter = sysconf(_SC_NPROCESSORS_ONLN);
	
	if(argc >= 2)
		thread_counter = strtoull(argv[1], NULL, 10);	
	
	pthread_t* threads = (pthread_t*) malloc(thread_counter * sizeof(pthread_t));
	
	for(size_t index = 0; index < thread_counter; ++index)	
		pthread_create(&threads[index], NULL, run, (void*)index);
	
	for(size_t index = 0; index < thread_counter; ++index)
		pthread_join(threads[index], NULL);
	
	printf("Hello world from main thread\n");
	
	free(threads);
	
	return 0;
}
