#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <semaphore.h>

typedef struct{
    size_t size;
    int** sudoku;
    int** aux_sudoku;
    pthread_mutex_t mutex;
    size_t thread_count;
    size_t rows;
    size_t columns;
    bool valid;
    pthread_barrier_t start_barrier;
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
void errors_local_block(size_t start_row, size_t start_column, shared_data_t* shared, private_data_t* private);

int main (int argc, char* argv[]){
    shared_data_t* shared_data = (shared_data_t*) calloc(1,sizeof (shared_data_t));
    if(shared_data ==NULL)
        return (void)(fprintf(stderr, "no se pudo asignar mem")) , 1;
    

    pthread_mutex_init(&shared_data->mutex, NULL);
    shared_data->thread_count = (size_t)sysconf(_SC_NPROCESSORS_ONLN);
    if(argc >= 2)
        shared_data->thread_count = strtoull(argv[1], NULL, 10);
    

    int error = read_sudoku(shared_data);
    if (error)
        return 1;
    
    shared_data->valid = true;

    shared_data->aux_sudoku = create_sudoku(shared_data);
    for(size_t rows = 0; rows < shared_data->size*shared_data->size; ++rows){
        for (size_t columns = 0; columns < shared_data->size*shared_data->size; ++columns) {
            shared_data->aux_sudoku[rows][columns] = 0;
        }
    }
    pthread_barrier_init(&shared_data->start_barrier, NULL, shared_data->thread_count);
    
    create_threads(shared_data);
    pthread_mutex_destroy(&shared_data->mutex);
    destroy_sudoku(shared_data->sudoku, shared_data->size * shared_data->size);
    
    if(shared_data->valid)
        printf("valid");
    
    destroy_sudoku(shared_data->sudoku, pow(shared_data->size,2));
    destroy_sudoku(shared_data->aux_sudoku, pow(shared_data->size,2));
    free(shared_data);
    return 0;
}

int read_sudoku(shared_data_t* shared_data){
    if(scanf ("%zu", &shared_data->size) != 1){
        return 1;
    }
    getchar();

    shared_data->sudoku = create_sudoku(shared_data);
    for(size_t rows = 0; rows < shared_data->size*shared_data->size; ++rows){
        for (size_t columns = 0; columns < shared_data->size*shared_data->size; ++columns) {
            scanf("%d", &shared_data->sudoku[rows][columns]);
        }
        getchar();
    }
    return 0;
}

int** create_sudoku (shared_data_t* shared_data){
    int** sudoku = calloc(pow(shared_data->size,2), sizeof(int*));
    if(sudoku == NULL)
        return NULL;
    getchar();

    for (size_t rows = 0; rows < shared_data->size*shared_data->size; ++rows){
        if( (sudoku[rows] = calloc (pow(shared_data->size,2), sizeof(int))) == NULL)
            return (void)(destroy_sudoku(sudoku, rows)), NULL;
    }
    return sudoku;
}

void destroy_sudoku(int** sudoku, const size_t rows){
    //printf("Prueba\n");
    assert (sudoku);
    for(size_t row = 0; row < rows; row++)
        free (sudoku[row]);
    free(sudoku);
}

int create_threads(shared_data_t* shared_data){
    pthread_t* threads = (pthread_t*) malloc (shared_data->thread_count * sizeof(pthread_t));
    
    if(threads == NULL){
        return (void)fprintf(stderr, "error: could not allocate memory for %zu threads\n", shared_data->thread_count), 2;
    }
    private_data_t* private_data =(private_data_t*) calloc(shared_data->thread_count, sizeof(private_data_t));

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
    pthread_barrier_wait(&shared->start_barrier);
    if(private->thread_num == 0){
        for(size_t rows = 0; rows < shared->size*shared->size; ++rows){
            for (size_t columns = 0; columns < shared->size*shared->size; ++columns) {
                if(shared->aux_sudoku[rows][columns] != 0){
                    printf("r%zu,%zu\n", rows+1, columns+1);
                }
                shared->aux_sudoku[rows][columns] = 0;
            }
        }
    }
    pthread_barrier_wait(&shared->start_barrier);

    find_errors_columns(private, shared);

    pthread_barrier_wait(&shared->start_barrier);
    if(private->thread_num == 0){
        for(size_t rows = 0; rows < shared->size*shared->size; ++rows){
            for (size_t columns = 0; columns < shared->size*shared->size; ++columns) {
                if(shared->aux_sudoku[rows][columns] != 0){
                    printf("c%zu,%zu\n", rows+1, columns+1);
                    
                }
                shared->aux_sudoku[rows][columns] = 0;
            }
        }
    }
    pthread_barrier_wait(&shared->start_barrier);

    find_errors_block(private, shared);
    pthread_barrier_wait(&shared->start_barrier);
    if(private->thread_num == 0){
        for(size_t rows = 0; rows < shared->size*shared->size; ++rows){
            for (size_t columns = 0; columns < shared->size*shared->size; ++columns) {
                if(shared->aux_sudoku[rows][columns] != 0){
                    printf("b%zu,%zu\n", rows+1, columns+1);
                }
                shared->aux_sudoku[rows][columns] = 0;
            }
        }
    }
    pthread_barrier_wait(&shared->start_barrier);
    return NULL;
}

void find_errors_rows(private_data_t* private, shared_data_t* shared){
    int* seen = (int*) malloc (shared->size*shared->size * sizeof(int)+1);
    assert (seen);
    for(size_t index = 0; index < shared->size*shared->size; ++ index){
        seen[index] = 0;
    }
    for (size_t rows = private->thread_num; rows < shared->size*shared->size; rows += shared->thread_count){
        for (size_t columns = 0; columns < shared->size*shared->size; columns++){
            if(shared->sudoku[rows][columns] != 0 && seen[shared->sudoku[rows][columns]-1] != 0){
                
                    pthread_mutex_lock(&shared->mutex);
                    shared->valid = false;
                    shared->aux_sudoku[rows][columns] = shared->sudoku[rows][columns];
                    pthread_mutex_unlock (&shared->mutex);
            }
            else{
                    seen[shared->sudoku[rows][columns] -1 ] = shared->sudoku[rows][columns];
            }
        }
        for(int index = 0; index < pow (shared->size, 2); ++index)
            seen[index] = 0;
    }
    free(seen);
}

void find_errors_columns(private_data_t* private, shared_data_t* shared){
    int* seen = (int*) malloc (shared->size*shared->size * sizeof(int));
    assert (seen);
    for(size_t index = 0; index < shared->size*shared->size; ++ index){
        seen[index] = 0;
    }
    for (size_t columns = private->thread_num; columns < shared->size*shared->size; columns += shared->thread_count){
        for (size_t rows = 0; rows < shared->size*shared->size; rows++){
            if(shared->sudoku[rows][columns] != 0 && seen[shared->sudoku[rows][columns]-1] != 0){
                
                    pthread_mutex_lock(&shared->mutex);
                    shared->valid = false;
                    shared->aux_sudoku[rows][columns] = shared->sudoku[rows][columns];
                    pthread_mutex_unlock (&shared->mutex);
            }
            else{
                    seen[shared->sudoku[rows][columns] -1 ] = shared->sudoku[rows][columns];
            }
        }
        for(int index = 0; index < pow (shared->size, 2); ++index)
            seen[index] = 0;
    }
    free (seen);
}

void find_errors_block (private_data_t* private, shared_data_t* shared){
    for (size_t start_row = private->thread_num * shared->size; start_row < pow(shared->size,2); start_row += shared->thread_count*shared->size){
        for (size_t start_column = 0; start_column < pow(shared->size, 2); start_column += shared->size){
            errors_local_block(start_row, start_column, shared, private);
        }
    }
}

void errors_local_block(size_t start_row, size_t start_column, shared_data_t* shared, private_data_t* private){
    int* seen = (int*) malloc (shared->size*shared->size * sizeof(int));
    assert(seen);
    for (int i = 0; i < pow(shared->size, 2); ++i){
        seen[i] = 0;
    }
    for (size_t rows= start_row; rows < start_row + (shared->size); ++rows){
        for (size_t columns = start_column; columns < start_column + (shared->size); ++columns){
            if(shared->sudoku[rows][columns] != 0 && seen[shared->sudoku[rows][columns]-1] != 0){
                pthread_mutex_lock(&shared->mutex);
                shared->valid = false;
                shared->aux_sudoku[rows][columns] = shared->sudoku[rows][columns];
                pthread_mutex_unlock (&shared->mutex);
            }
            else{
                seen[shared->sudoku[rows][columns]-1] = shared->sudoku[rows][columns];
            }
        }
    }
    free (seen);
}