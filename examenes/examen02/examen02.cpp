#include <mpi.h>
#include <omp.h>
#include <cstdlib>
#include <string>
#include <stdbool.h>
#include <stddef.h>

typedef struct
{
	int rows;
	int columns;
	int area;
	int cloud_id;
}coordenadas; 


double ** crear_image(std::string nombre, int rows, int columns);
double ** crear_mirror(int rows, int columns);
void search_clouds(double ** image, double ** mirror, int my_start, int my_finish, coordenadas *array_coordenadas, int global_rows, int a, int b);
void expand_cloud(int rows, int col, coordenadas* array_coordenadas, double a, double b, double**mirror, double** image, int cloud_count);
void print_clouds(coordenadas* coordenadas);
bool exist(int row, int col, int global_rows, int global_col);
int calculate_start(int worker_id, int workers, int finish, int begin);

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	int my_rank = -1;
	int process_count = -1;
	coordenadas *array_coordenadas = (coordenadas*) malloc(sizeof(coordenadas)*4);
	

	int rows = 0;
	int columns = 0;
	
	const int nitems=4;
	int blocklengths[4] = {1, 1, 1, 1};
    MPI_Datatype types[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Datatype coordenadas_nube;
    MPI_Aint offsets[4];

    offsets[0] = offsetof(coordenadas, rows);
    offsets[1] = offsetof(coordenadas, columns);
    offsets[2] = offsetof(coordenadas, area);
    offsets[3] = offsetof(coordenadas, cloud_id);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &coordenadas_nube);
    MPI_Type_commit(&coordenadas_nube);

	double a = atof(argv[2]);
	double b = atof(argv[3]);
	scanf("%d", &rows);
	scanf("%d", &columns);

	double ** image = crear_image(argv[1], rows, columns);
	double ** mirror = crear_mirror (rows, columns);

	char hostname[MPI_MAX_PROCESSOR_NAME];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);

	int my_start = calculate_start(my_rank, process_count, rows, 0);
	int my_finish = calculate_start(my_rank+1, process_count, rows, 0);

	search_clouds (image, mirror, my_start, my_finish, array_coordenadas, rows, a, b);

	if(my_rank == 0){
		int size = 0;
		
		coordenadas *array_buffer = (coordenadas*) malloc (sizeof(coordenadas)*size);
			
		for (int index = 0; index < process_count; ++index){
			MPI_Recv(&size, 1, MPI_INT, index, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&array_buffer, 1, coordenadas_nube, index, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			double *column = (double*) malloc(sizeof(double)*rows);
			double *bcolumns = (double*) malloc(sizeof(double)*rows);
			MPI_Recv(&bcolumns, 1, MPI_DOUBLE, index, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
			for(int cindex = 0; cindex < columns; cindex++){
				column[cindex] = mirror [my_finish][cindex];
			}

			for(int cindex = 0; cindex < columns; cindex++){
				if(column[cindex] != 0 && bcolumns[cindex] != 0){
					if(array_coordenadas[(int)(column[cindex] -1)].rows < array_buffer[(int)(bcolumns[cindex]-1)].rows){
						array_coordenadas[(int)(column[cindex]-1)].area += array_buffer[(int)(bcolumns[cindex]-1)].area;
						array_buffer[(int)(bcolumns[cindex]-1)].cloud_id = 0;
					}
					else{
						if (array_coordenadas[(int)(column[cindex] -1)].rows > array_buffer[(int)(bcolumns[cindex]-1)].rows){
							array_buffer[(int)(bcolumns[cindex]-1)].area += array_coordenadas[(int)(column[cindex]-1)].area;
							array_coordenadas[(int)(bcolumns[cindex]-1)].cloud_id = 0;
						}
						else{
							array_coordenadas[(int)(column[cindex]-1)].area += array_buffer[(int)(bcolumns[cindex]-1)].area;
							array_buffer[(int)(bcolumns[cindex]-1)].cloud_id = 0;
						}
					}
				}
			}
			int new_tamanio = size + sizeof(array_coordenadas);
			coordenadas *new_array = (coordenadas*) malloc(sizeof(int)*new_tamanio);
			int contador1 = 0;
			int contador2 = 0;
			int itr = 0;

			while(contador1 < (int)sizeof(array_coordenadas) || contador2 < size){
				if(array_coordenadas[contador1].cloud_id < array_buffer[contador2].cloud_id	){
					new_array[itr] = array_coordenadas[contador1];
					contador1++;
				}
				else{
					new_array[itr] = array_buffer[contador2];
					contador2++;
				}
				itr++;
			}
			coordenadas* temp = array_coordenadas;
			array_coordenadas = new_array;
			free(temp);
 			}
 		}
 		else{
 			int size = (int)sizeof(array_coordenadas);
 			coordenadas *array_buffer = array_coordenadas;
 			//double *column = (double*) malloc(sizeof(double)*rows);
 			//column = mirror[my_finish];
 			MPI_Send(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
 			MPI_Send(&array_buffer, 1, coordenadas_nube, 0, 0, MPI_COMM_WORLD);
 			MPI_Send(&size, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
 		}

 		if(my_rank == 0){
 			print_clouds(array_coordenadas);
 		MPI_Finalize();
		return 0;
}
}

double ** crear_mirror(int rows, int columns){
	double ** matriz = (double**)calloc(columns, sizeof(double*));
	for(int index = 0; index < rows; index++){
		matriz[index] = (double*)calloc(columns, sizeof(double));
	}
	return matriz;
}

double ** crear_image(std::string nombre, int rows, int columns){
	double ** matriz = crear_mirror(rows, columns);
	(void)(nombre);
	/*
	archivo = open(nombre);
	for(int rindex = 0; rindex < rows; ++index){
		scanf(%f, matriz[rindex][cindex], archivo);
	}*/
	return matriz;
}

void search_clouds(double ** image, double ** mirror, int my_start, int my_finish, coordenadas *array_coordenadas, int global_rows, int a, int b){
	(void)image;
	int cloud_count = 0;
	for (int rows = 0; rows < global_rows; ++rows){
		for (int col = my_start; col < my_finish; ++col){
			if(mirror[rows][col] == 0){
				array_coordenadas[cloud_count].cloud_id = cloud_count;
				array_coordenadas[cloud_count].area = 0;
				expand_cloud(rows, col, array_coordenadas, a, b, mirror, image, cloud_count);
				cloud_count++;
			}
			else
				mirror[rows][col] = -1;
		}
	}
}


void expand_cloud(int rows, int col, coordenadas* array_coordenadas, double a, double b, double**mirror, double** image, int cloud_count){
	int global_rows = 0;
	int global_col = 0;
	if (!exist(rows, col, global_rows, global_col))
		return;
	if (mirror[rows][col] != 0)
		return;
	if (a <= image[rows][col] && b >= image[rows][col]){
		mirror[rows][col] = array_coordenadas[cloud_count].cloud_id;
		array_coordenadas[cloud_count].area++;
		expand_cloud(rows, col, array_coordenadas, a, b, mirror, image, cloud_count);
		expand_cloud(rows, col, array_coordenadas, a, b, mirror, image, cloud_count);
		expand_cloud(rows, col, array_coordenadas, a, b, mirror, image, cloud_count);
		expand_cloud(rows, col, array_coordenadas, a, b, mirror, image, cloud_count);	
	}
	else
		mirror[rows][col] = -1;
}

void print_clouds(coordenadas* coordenadas){
	for(int index = 0; index < (int)sizeof(coordenadas); index++){
		printf("%d: %d", index, coordenadas[index].area);
	}
}

bool exist(int row, int col, int global_rows, int global_col){
	return row >= 0 && row < global_rows && col >= 0 && col <= global_col;
}

int calculate_start(int worker_id, int workers, int finish, int begin)
{
	int range = finish - begin;
	return begin + worker_id * (range / workers) + std::min(worker_id, range % workers);
}