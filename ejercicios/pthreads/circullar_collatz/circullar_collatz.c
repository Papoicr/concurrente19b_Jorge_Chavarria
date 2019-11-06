#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct{
	size_t worker_count;
	size_t max_steps;
	size_t current_step;
	size_t number_count;
	size_t* numbers;
	pthread_barrier_t barrier;
	pthread_t* workers;
	pthread_mutex_t mutex;
}shared_data_t;

typedef struct{
	size_t id;
	size_t next;
	size_t prev;
	size_t number_count;
	size_t current_step;
	size_t* numbers;
	shared_data_t* shared;
}private_data_t;

bool finish(size_t* numbers, size_t size)
{
	for(size_t index = 0; index < size; ++index)
	{
		if(numbers[index] != 1)
            return false;
	}
	return true;
}

void* calculate(void* data)
{
	private_data_t* private = (private_data_t *) data;
	shared_data_t* shared = private->shared;
	
	while ( private->current_step < shared->max_steps && finish(private->numbers, private->number_count))
	{
		if (private->numbers[private->id] % 2 == 0 ){
			pthread_barrier_wait(&shared->barrier); 
			private->numbers[private->id] = private->numbers[private->id] / 2;
			pthread_barrier_wait(&shared->barrier);
		}
		else{
			pthread_barrier_wait(&shared->barrier); 
			private->numbers[private->id] = private->numbers[private->id] * private->numbers[private->prev] + private->numbers[private->next];
			pthread_barrier_wait(&shared->barrier);
		}


		if ( private->id == 0 )
			++private->current_step;
	}
	return NULL;
}

int main()
{
	
	shared_data_t* shared = (shared_data_t*) calloc(1, sizeof(shared_data_t)); 
	if (shared == NULL)
		return (void)fprintf(stderr, "error: could not allocate shared memory\n"), 1;
		
	scanf("%zu %zu %zu\n", &shared->number_count, &shared->max_steps, &shared->worker_count);
	
	
	shared->numbers = malloc( shared->number_count * sizeof(size_t) );
	for ( size_t index = 0; index < shared->number_count; ++index )
		scanf("%zu", &shared->numbers[index]);
	printf ("Prueba");
	pthread_barrier_init(&shared->barrier, NULL, (unsigned) shared->worker_count);
	pthread_mutex_init(&shared->mutex, /*attr*/ NULL);

	shared->workers = malloc(shared->worker_count * sizeof(pthread_t));
	printf ("Prueba");
	private_data_t* private_data = calloc(shared->worker_count, sizeof(private_data_t));
	if(private_data == NULL)
		return (void)fprintf(stderr, "error: could not allocate private memory for %zu threads\n", shared->worker_count), 1;
	
	printf ("Prueba");
	
	for ( size_t index = 0; index < shared->worker_count; ++index ){
		private_data[index].id = index;
		printf ("Prueba");
		private_data[index].prev = (index + shared->number_count - 1) % shared->number_count;
		
		printf ("Prueba");
		private_data[index].next = (index + 1) % shared->number_count;
		printf ("Prueba");
		private_data[index].number_count = shared->number_count;
		printf ("Prueba");
		private_data[index].current_step = shared->current_step;
		printf ("Prueba");
		private_data[index].numbers = shared->numbers;
		printf ("Prueba");
		private_data[index].shared = shared;
		printf ("Prueba");
		pthread_create(&shared->workers[index], NULL, calculate, (void*)index);
	}
	
	for ( size_t index = 0; index < shared->worker_count; ++index )
		pthread_join(shared->workers[index], NULL);

	pthread_barrier_destroy(&shared->barrier);

	if ( shared->current_step > shared->max_steps )
		printf("No converge in %zu steps", shared->max_steps);
	else
		printf("Converged in %zu steps", shared->current_step);

	return 0;
}
