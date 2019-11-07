#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

typedef struct{
    size_t size;
    int** sudoku;
    pthread_mutex_t mutex;
    bool valid;
    size_t thread_count;
} shared_data_t;

typedef struct{
    shared_data_t* shared_data;
    size_t thread_num;
} private_data_t;

int main (int argc, char* argv[]){
    shared_data* shared_data = (shared_data_t*) calloc(1,sizeof (shared_data_t));
    if(shared_data ==NULL)
        return (void)(fprintf("no se pudo asignar mem"),1;
    pthread_mutex_init(&shared_data->mutex);
    shared_data->thread_count = (size_t)sysconf(_SC_NPROCESSORS_ONLN);
    if(argc >= 2)
        shared_data->size = strtoull(argv[1], NULL, 10);
    error = read_sudoku(shared_data);
    if (error)
        return 1;
    shared_data->valid = true;
    create_threads(shared_data);
    pthread_mutex_destroy(&shared_data->mutex);
    destroy_sudoku(shared_data->sudoku);
    if(shared_data->valid)
            printf("valid");
    free(shared_data);
    return 0;
}

int read_sudoku(shared_data_t shared_data){
    if(scanf ("%zu", shared_data->size)
        return 1;
    shared_data->sudoku = create_sudoku(shared_data)
    for(rows = 0; rows < shared_data->size*shared_data->size; ++rows)
        for (columns = 0; columns < shared_data->size*shared_data->size; ++columns) {
            shared_data->sudoku[rows][columns] = getchar();
            getchar();
        }
    getchar()
}

int** create_sudoku (shared_data_t shared_data){
	int** sudoku = calloc(rows, sizeof(int*))
	if(sudoku == NULL){
		for (size_t rows = 0; rows < shared_data->size*shared_data->size; ++rows){
			if(calloc (columes, sizeof(int*)) == NULL
				return destroy_sudoku(sudoku, rows);
		
	}	
	return sudoku
}

destroy_sudoku(int** sudoku; size){
	assert (sudoku);
	for(rows = 0; rows < size; rows++)
		free (sudoku[rows]);
	free(sudoku)
}

int create_threads(shared_data_t shared_data){
	pthread* threads = (pthread_t*) malloc (shared_data->thread_count)*sizeof(pthread);
	if(threads == NULL)
		return 1;
	private_data_t* private_data =(private_data_t*) calloc (shared_data->thread_count, sizeof(private))
	
	for (index = 0; index < shared_data->thread_count; index++){
			private_data[index].thread_num = index;
			private_data[index].shared_data = shared_data;
			pthread_create(&threads[indexx], NULL, run, &private_data[index];
	}
	for (index = 0; index < shared_data; index++){
		pthread_join(&threads[index]);
	}
	free(private_data);
	free(threads);
	return 0;
}

void* run (void* data){
	private_data_t private = (private_data_t*)(data);
	shared_data_t shared = private->shared;
	find_errors_rows(private, shared);
	find_errors_columns(private, shared);
	find_errors_block(private, shared);
	return NULL;
}

void find_errors_rows(private_data_t private; shared_data_t shared){
	int* seen = (int*) malloc (shared->size*shared->size);
	assert (seen);
	for(size_t index = 0; index < shared->size*shared->size; ++ index){
		seen[index] = 0;
	}
	for (size_t rows = private->thread_num; rows < shared->size*shared->size; rows += shared_data->thread_count){
		for size_t columns = 0; columns < shared->size*shared->size; columns++){
			if(shared->sudoku[rows][columns] != 0){
				for(size_t index = 0; index < pow(shared->size); ++ index){
					if (shared->sudoku[rows][columns] == seen[index]);
					pthread_mutex_lock(&shared->mutex);
					printf("r (%zu, %zu)", rows, columns);
					shared->valid = false;
					pthread_mutex_unlock (&shared->mutex);
					}
					else{
						seen[shared->sudoku[rows][columns] -1 ] = shared->sudoku[rows][columns];
					} 
				}
			}
		}
		for(int index = 0; index < pow (shared->size); ++index){
			seen[index] = 0;
		}
		free (seen);
	
}

void find_errors_columns(private_data_t private; shared_data_t shared){
	int* seen = (int*) malloc (shared->size*shared->size);
	assert (seen);
	for(size_t index = 0; index < shared->size*shared->size; ++ index){
		seen[index] = 0;
	}
	for (size_t columns = private->thread_num; columns < shared->size*shared->size; columns += shared_data->thread_count){
		for size_t rows = 0; rows < shared->size*shared->size; rows++){
			if(shared->sudoku[rows][columns] != 0 && seen[shared->sudoku[rows][columns]-1] != 0){
				pthread_mutex_lock(&shared->mutex);
				printf("c (%zu, %zu)", rows, columns);
				shared->valid = false;
				pthread_mutex_unlock (&shared->mutex);
				}
				else{
					seen[shared->sudoku[rows][columns] -1 ] = shared->sudoku[rows][columns];
				} 
				}
			}
		}
		for(int index = 0; index < pow (shared->size); ++index)
			seen[index] = 0;
		}
		free (seen);
}

void find_errors_blocks (private_data_t private; shared_data_t shared){
	for (size_t start_row = private->thread_num; start_row < pow(shared->size); start_row +=shared->size){ //arreglar mapeo multiplicando por thread_count
		for (size_t start_column = private->thread_num; start_column < pow(shared->size); start_column +=shared->size){
			error_local_bloc(start_row, start_column);
		}
	}
}

void errors_local__block(size_t start_row, size_t start_column, shared_data_t shared){
	for (size_t x= start_row; x < star_row*shared->size+shared->size -1 ; ++x){
		int* seen = (int*) malloc (shared->size*shared->size);
		assert(seen)
		for (int i = 0; i <pow (shared->size); ++i)
			seen[i] = 0;
	}
	for (size_t y = start_column; y < star_column * shared->size+share->size-1; ++y){
		if(shared->sudoku[rows][columns] != 0 && seen[shared->sudoku[rows][columns]-1] != 0){
			pthread_mute_lock(&shared->mutex);
			printf("b%zu%zu", x, y)
			shared->valid = false
			pthread_mutex_unlock(shared->mutex);
		}
		else{
			seen[shared->sudoku[x][y]-1] = shard->sudoku[x][y];
		}
		for(int i = 0; i < pow(shared->size); ++i){
			seen[i] = 0;
		}
		free (seen);
	
	
}
