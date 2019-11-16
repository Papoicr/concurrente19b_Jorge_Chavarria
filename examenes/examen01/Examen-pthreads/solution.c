#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

typedef struct{
    size_t size;
    int** sudoku;
    pthread_mutex_t mutex;
    size_t thread_count;
    size_t rows;
    size_t columns;
    bool valid;
} shared_data_t;

typedef struct{
    shared_data_t* shared;
    size_t thread_num;
} private_data_t;

int create_threads(shared_data_t* shared_data);
int read_sudoku(shared_data_t* shared_data);
int** create_sudoku (shared_data_t* shared_data);
void destroy_sudoku(int** sudoku, size_t size);
void* run (void* data);
void find_errors_rows(private_data_t* private, shared_data_t* shared);
void find_errors_columns(private_data_t* private, shared_data_t* shared);
void find_errors_block(private_data_t* private, shared_data_t* shared);
void errors_local_block(size_t start_row, size_t start_column, shared_data_t* shared);

int main (int argc, char* argv[]){
    shared_data_t* shared_data = (shared_data_t*) calloc(1,sizeof (shared_data_t));
    if(shared_data ==NULL)
        return (void)(fprintf(stderr, "no se pudo asignar mem")) , 1;
    pthread_mutex_init(&shared_data->mutex, NULL);
    shared_data->thread_count = (size_t)sysconf(_SC_NPROCESSORS_ONLN);
    if(argc >= 2)
        shared_data->size = strtoull(argv[1], NULL, 10);
    int error = read_sudoku(shared_data);
    if (error)
        return 1;
    shared_data->valid = true;
    create_threads(shared_data);
    pthread_mutex_destroy(&shared_data->mutex);
    destroy_sudoku(shared_data->sudoku, shared_data->size * shared_data->size);
    if(shared_data->valid)
            printf("valid");
    free(shared_data);
    return 0;
}

int read_sudoku(shared_data_t* shared_data){
    if(scanf ("%zu", &shared_data->size))
        return 1;
    shared_data->sudoku = create_sudoku(shared_data);
    for(size_t rows = 0; rows < shared_data->size*shared_data->size; ++rows)
        for (size_t columns = 0; columns < shared_data->size*shared_data->size; ++columns) {
            shared_data->sudoku[rows][columns] = getchar();
            getchar();
        }
    getchar();
    return 0;
}

int** create_sudoku (shared_data_t* shared_data){
    int** sudoku = calloc(shared_data->rows, sizeof(int*));
	if(sudoku == NULL){
		for (size_t rows = 0; rows < shared_data->size*shared_data->size; ++rows){
            if(calloc (shared_data->columns, sizeof(int*)) == NULL)
                return destroy_sudoku(sudoku, rows), NULL;
        }
    }
    return sudoku;
}

void destroy_sudoku(int** sudoku, size_t size){
	assert (sudoku);
    for(size_t rows = 0; rows < size; rows++)
		free (sudoku[rows]);
    free(sudoku);
}

int create_threads(shared_data_t* shared_data){
    pthread_t* threads = (pthread_t*) malloc (shared_data->thread_count*sizeof(pthread_t));
	if(threads == NULL)
		return 1;
    private_data_t* private_data =(private_data_t*) calloc (shared_data->thread_count, sizeof(private_data_t));
	
    for (size_t index = 0; index < shared_data->thread_count; index++){
			private_data[index].thread_num = index;
            private_data[index].shared = shared_data;
            pthread_create(&threads[index], NULL, run, &private_data[index]);
	}
    for (size_t index = 0; index < shared_data->thread_count; index++){
        pthread_join(threads[index], NULL);
	}
	free(private_data);
	free(threads);
	return 0;
}

void* run (void* data){
    private_data_t* private = (private_data_t*)(data);
    shared_data_t* shared = private->shared;
	find_errors_rows(private, shared);
	find_errors_columns(private, shared);
	find_errors_block(private, shared);
	return NULL;
}

void find_errors_rows(private_data_t* private, shared_data_t* shared){
	int* seen = (int*) malloc (shared->size*shared->size);
	assert (seen);
	for(size_t index = 0; index < shared->size*shared->size; ++ index){
		seen[index] = 0;
	}
    for (size_t rows = private->thread_num; rows < shared->size*shared->size; rows += shared->thread_count){
        for (size_t columns = 0; columns < shared->size*shared->size; columns++){
			if(shared->sudoku[rows][columns] != 0){
                for(size_t index = 0; index < pow(shared->size, 2); ++ index){
                    if (shared->sudoku[rows][columns] == seen[index]){
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
        for(int index = 0; index < pow(shared->size, 2); ++index){
			seen[index] = 0;
		}
		free (seen);
	
    }
}

void find_errors_columns(private_data_t* private, shared_data_t* shared){
    int* seen = (int*) malloc (shared->size*shared->size);
	assert (seen);
	for(size_t index = 0; index < shared->size*shared->size; ++ index){
		seen[index] = 0;
	}
    for (size_t columns = private->thread_num; columns < shared->size*shared->size; columns += shared->thread_count){
        for (size_t rows = 0; rows < shared->size*shared->size; rows++){
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
    for(int index = 0; index < pow (shared->size, 2); ++index)
        seen[index] = 0;
    free (seen);
}

void find_errors_block (private_data_t* private, shared_data_t* shared){
    for (size_t start_row = private->thread_num; start_row < pow(shared->size,2); start_row +=shared->size){ //arreglar mapeo multiplicando por thread_count
        for (size_t start_column = private->thread_num; start_column < pow(shared->size, 2); start_column +=shared->size){
            errors_local_block(start_row, start_column, shared);
		}
	}
}

void errors_local_block(size_t start_row, size_t start_column, shared_data_t* shared){
    for (size_t x= start_row; x < start_row*shared->size+shared->size -1 ; ++x){
		int* seen = (int*) malloc (shared->size*shared->size);
        assert(seen);
        for (int i = 0; i < pow(shared->size, 2); ++i){
			seen[i] = 0;
        }
        for (size_t y = start_column; y < start_column * shared->size+shared->size-1; ++y){
        if(shared->sudoku[x][y] != 0 && seen[shared->sudoku[x][y]-1] != 0){
            pthread_mutex_lock(&shared->mutex);
            printf("b%zu%zu", x, y);
            shared->valid = false;
            pthread_mutex_unlock(&shared->mutex);
		}
		else{
            seen[shared->sudoku[x][y]-1] = shared->sudoku[x][y];
		}
        for(int i = 0; i < pow(shared->size,2); ++i){
			seen[i] = 0;
		}
		free (seen);
        }
    }
}
