/**
*	Profe, al final si cambie de division en columnas a division en fila, por que intené dejarlo en columnas pero se
*	complicaba mucho, sin embargo dejé cosas como el struct por que eran necesarias para la solucion original,
*   de igual manera hay un error que no logré encontrar que se genera luego de la comunicación entre procesos
*	en general, los procesos se comunica bien pero al finalizar el error que tira dice que es un problema con un free(), 
*	busque el error por tantas horas que al final opté por quitar todos los free (sabiendo que esto genera fugas) 
*	y aun asi el error sigue; si a la hora de revisar el codigo sabe o encuentra que lo genera agradeceria que me avisara
*   ya que no quiero quedar con la duda.
*/

#include <mpi.h>
#include <omp.h>
#include <cstdlib>
#include <string>
#include <stdbool.h>
#include <stddef.h>
#include <vector>
#include <iostream>
#include <fstream>

typedef struct
{
	int rows;
	int columns;
	int area;
	int cloud_id;
}coordenadas; 


double **crear_image(std::string nombre, int rows, int columns);
int **crear_mirror(int rows, int columns);
void search_clouds(double **image, int **mirror, int my_start, int my_finish, std::vector<int>* array_coordenadas, int global_col, int a, int b);
bool exist(int row, int col, int global_rows, int global_col);
void expand_cloud(double ** image, int ** mirror, int my_start, int my_finish, std::vector<int>* array_coordenadas, int global_col, double a, double b, int cloud_count, int rows, int col);
void print_clouds(std::vector<int> coordenadas);
int calculate_start(int worker_id, int workers, int finish, int begin);

int **crear_mirror(int rows, int columns){
	int ** matriz = new int *[rows];
	for(int index = 0; index < rows; index++){
		matriz[index] = new int[columns];
	}
	return matriz;
}

double **crear_image(std::string nombre, int rows, int columns){
	double ** matriz = new double *[rows];
	for(int index = 0; index < rows; index++){
		matriz[index] = new double[columns];
	}
	(void)(nombre);
	return matriz;
}

void search_clouds(double ** image, int ** mirror, int my_start, int my_finish, std::vector<int>* array_coordenadas, int global_col, double a, double b){
	//por motivos que no entiendo, si no me aseguro de que la matriz esta en 0 las 2 primeras filas de mirror toman valores extraños, esto solo sucede si es mas de un proceso
	for (int i = 0; i < 11; ++i ){
		for (int j = 0; j < 8; ++j){ //se cae si no se escribe el numero maximo
			mirror[i][j] = 0;
		}
	}

	int cloud_count = 0;
	for (int rows = my_start; rows < my_finish; ++rows){
		for (int col = 0; col < global_col; ++col){
			if(mirror[rows][col] == 0){
				if( image[rows][col] >= a && image[rows][col] <= b){
					cloud_count++;	
					array_coordenadas->push_back(0);
					expand_cloud(image, mirror, my_start, my_finish, array_coordenadas, global_col, a, b, cloud_count, rows, col);
				}
				else
					mirror[rows][col] = -1;
			}
		}
	}
}

bool exist(int row, int col, int global_rows, int my_start, int my_finish){
	return row >= 0 && row < global_rows && col >= 0 && col <= 8;
}

void expand_cloud(double ** image, int ** mirror, int my_start, int my_finish, std::vector<int>* array_coordenadas, int global_rows, double a, double b, int cloud_count, int rows, int col){
	if (!exist(rows, col, global_rows, my_start, my_finish))
		return;
	if (mirror[rows][col] != 0)
		return;
	if (image[rows][col] >= a && image[rows][col] <= b){
		array_coordenadas[0][cloud_count-1]++;
		mirror[rows][col] = cloud_count;
		expand_cloud(image, mirror, my_start, my_finish, array_coordenadas, global_rows, a, b, cloud_count, rows-1, col);
		expand_cloud(image, mirror, my_start, my_finish, array_coordenadas, global_rows, a, b, cloud_count, rows, col-1);
		expand_cloud(image, mirror, my_start, my_finish, array_coordenadas, global_rows, a, b, cloud_count, rows+1, col);
		expand_cloud(image, mirror, my_start, my_finish, array_coordenadas, global_rows, a, b, cloud_count, rows, col+1);	
	}
	else
		mirror[rows][col] = -1;
}

void print_clouds(std::vector<int> coordenadas){
	for(int index = 0; index < (int)coordenadas.size(); index++){
		std::cout << index+1  << " " << coordenadas[index] << std::endl;
	}
}

int calculate_start(int worker_id, int workers, int finish, int begin)
{
	int range = finish - begin;
	return begin + worker_id * (range / workers) + std::min(worker_id, range % workers);
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	int my_rank = -1;
	int process_count = -1;
	std::vector<int> array_coordenadas;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	char hostname[MPI_MAX_PROCESSOR_NAME];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);

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
	
	std::ifstream fe("file.txt");
	fe >> rows >> columns;

	int **mirror;	 
	mirror = crear_mirror (rows, columns);
	double **image;
	image = crear_image(argv[1], rows, columns);

	for (int i = 0; i < rows; ++i ){
		for (int j = 0; j < columns; ++j){
			fe >> image[i][j]; 
		}
	}
	fe.close();

	int my_start = calculate_start(my_rank, process_count, rows, 0);
	int my_finish = calculate_start(my_rank+1, process_count, rows, 0);

	search_clouds(image, mirror, my_start, my_finish, &array_coordenadas, rows, a, b);
	if(process_count > 1){
		if(my_rank > 0){
			int size = 0;
				
			MPI_Recv(&size, 1, MPI_INT, my_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			std::vector<int> array_buffer;
			array_buffer.resize(size);
			MPI_Recv(&array_buffer[0], array_buffer.size(), MPI_INT, my_rank-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			std::vector<int> vector_row;
			std::vector<int> brows;
			
			vector_row.resize(columns);

			for(int rindex = 0; rindex < columns; rindex++){
				brows.push_back(mirror[my_start][rindex]);
			}
			MPI_Recv(&vector_row[0], vector_row.size(), MPI_DOUBLE, my_rank-1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for(size_t rindex = 0; rindex < vector_row.size(); rindex++){
				if(vector_row[rindex] > 0 && brows[rindex] > 0){
					if(mirror[my_finish-1][rindex] == brows[rindex]){
						mirror[my_finish-1][rindex] = vector_row[rindex];
					}
					else{
						if(mirror[my_finish-1][rindex]>0)
							mirror[my_finish-1][rindex] += (int)array_coordenadas.size();
					}
					array_buffer[brows[rindex]] += array_coordenadas[vector_row[rindex]] ;
					array_coordenadas.erase(array_buffer.begin() + (brows[rindex]-1));
				}
				else{
					if(mirror[my_finish-1][rindex] > 0){
						mirror[my_finish-1][rindex] += array_coordenadas.size();
					}
				}
			}

			for(int itr = 0; itr < array_coordenadas.size(); ++itr){
				array_buffer.push_back(array_coordenadas[itr]);
			}
			
			if(my_rank < process_count-1){
				int size = (int)array_buffer.size();
	 			MPI_Send(&size, 1, MPI_INT, my_rank+1, 0, MPI_COMM_WORLD);
	 			MPI_Send(&array_buffer[0], array_buffer.size(), coordenadas_nube, my_rank+1, 1, MPI_COMM_WORLD);
	 			
	 			for(int rindex = 0; rindex < rows; rindex++){
					vector_row.push_back(mirror[my_finish][rindex]);
				}
	 			MPI_Send(&vector_row[0], vector_row.size(), MPI_DOUBLE, my_rank+1, 2, MPI_COMM_WORLD);
			}
	 	}
	 	else{
	 		std::vector<int> array_buffer = array_coordenadas;
	 		std::vector<int> vector_row;
	 		int size = (int)array_buffer.size();
			MPI_Send(&size, 1, MPI_INT, my_rank+1, 0, MPI_COMM_WORLD);
			MPI_Send(&array_buffer[0], array_buffer.size(), MPI_INT, my_rank+1, 1, MPI_COMM_WORLD);
		 	
			for(int rindex = 0; rindex < columns; rindex++){
				vector_row.push_back(mirror[my_finish][rindex]);
			}
	 		MPI_Send(&vector_row[0], vector_row.size(), MPI_DOUBLE, my_rank+1, 2, MPI_COMM_WORLD);
	 	}
	}

	if(my_rank == process_count-1){
		print_clouds(array_coordenadas);
	}

	MPI_Finalize();
	return 0;
}