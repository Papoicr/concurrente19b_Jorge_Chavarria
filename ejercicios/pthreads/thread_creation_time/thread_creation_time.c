#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

void* run(void* data)
{
	(void)data;
	return NULL;
}

int main (int argc, char* argv[])
{
	pthread_t thread;
	double time;
	size_t tests = 1; //cantidad de pruebas por default
	
	if( argc >= 2) //verificar cuantas pruebas quiere el usuario
		tests = strtoull(argv[1], NULL, 10); //si se cumple cambiar el valor de la variable 
		
	for(size_t i = 0; i < tests; i++)//realizar la cantidad de pruebas 
	{
		struct timespec start_time;
		clock_gettime(CLOCK_MONOTONIC, &start_time);
		
		pthread_create(&thread, NULL, run, NULL);
		pthread_join(thread, NULL);
		
		struct timespec finish_time;
		clock_gettime(CLOCK_MONOTONIC, &finish_time);
		
		if(time > (finish_time.tv_sec - start_time.tv_sec + 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec)) || i == 0)
			time = finish_time.tv_sec - start_time.tv_sec+ 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);
			
	}
	printf("%f\n", time);
	return 0;
}
