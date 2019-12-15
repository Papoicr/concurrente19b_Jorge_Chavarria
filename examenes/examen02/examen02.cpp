#include <mpi>
#include <omp>
#include <stdlib>
#include <cstring>
#include <stdbool>

typedef struct
{
	int rows;
	int columns;
	int area;
	int cloud_id;
}coordenadas; 

int main(int argc, char const *argv[])
{
	if (argc < 4)
		return 1;
	
	MPI_Init(&argc, &argv);
	int my_rank = -1;
	int process_count = -1;
	coordenadas array_coordenadas = (coordenadas) malloc(coordenadas*4);
	int rows = 0;
	int columns = 0;
	MPI_Datatype coordenadas;
	double a = argv[2];
	double b = argv[3];
	scanf(%d, rows)
	scanf(%d, columns)

	double ** image = crear_image(argv[1], rowsm columns);
	double ** mirror = crear_mirror (rows, columns);

	char hostname[MPI_MAX_PROCESSOR_NAME];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);

	int my_start = calculate_start(my_rank, process_count, rows, 0);
	int my_finish = calculate_start(my_rank+1, process_count, rows, 0);

	search_clouds (image, mirror, my_start, my_finish, array_coordenadas);


	if(my_rank == 0){
		int size = 0;
		for (int index = 0; index < process_count, ++index){
			IMP_Recv(&size, 1, MPI_INT, index, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			coordenadas array_buffer = (coordenadas*) malloc (coordenadas*size);
			IMP_Recv(&array_buffer, 1, MPI_coordenadas, index, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			double columns = (double*) malloc(double*rows);
			double bcolumns = (double*) malloc(double*rows);
			IMP_Recv(&bcolumns, 1, MPI_DOUBLE, index, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
			for(int cindex = 0; cindex < columns < cindex++){
				columns[cindex] = mirror [my_finish][cindex]
			}

			for(int cindex = 0; cindex < columns < cindex++){
				if(column[cindex] != 0 && bcolumn[cindex] != 0){
					if(array_coordenadas[column[cindex] -1].rows < array_buffer[bcolumn[cindex]-1]){
						array_coordenadas[column[cindex]-1].area += array_buffer[bcolumn[cindex]-1].area;
						array_buffer[bcolumn[cindex]-1].cloud_id = 0;
					}
					else{
						if (array_coordenadas[column[cindex] -1].rows < array_buffer[bcolumn[cindex]-1].rows){
							borde(array_coordenadas, array_buffer, cindex);
						}
						else
							borde(array_buffer, array_coordenadas, cindex);
					}
				}
			}
			int new_tamaño = size + size(array_coordenadas)
			int *new_tamaño = (int*) malloc(int*new_tamaño);
			int contador1 = 0;
			int contador2 = 0;
			int itr = 0;

			while(contador1 < size (arreglo_coordenadas) || contador2 < size){
				if(arreglo_coordenadas[contador1].cloud_id < arreglo_buffer[contador2]){
					new_array[itr] = arreglo_coordenadas[contador1];
					contador1++;
				}
				else{
					new_array[itr] = arreglo_buffer[contador2];
					contador2++;
				}
				itr++;
			}
			int* temp = &array_coordenadas
			array_coordenadas = new_array;
			free(temp);
 		}
 		else
 			int size = size(array_coordenadas);
 			coordenadas array_buffer = array_coordenadas;
 			double column = (double*) malloc(double*rows);
 			column = mirror[my_finish];
 			MPI_SEND(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
 			MPI_SEND(&array_buffer, 1, MPI_coordenadas, 0, 0, MPI_COMM_WORLD);
 			MPI_SEND(&size, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
 		}

 		if(my_rank == 0){
 			print_clouds(array_coordenadas);
 		MPI_Finalize();
		return 0;
}

double crear_mirror(int rows, int columns){
	double ** matriz;
	for(int index = 0; index < rows; index++){
		matriz[index] = (double*) calloc(double*columns, 0);
	return matriz;

double crear_image(string nombre, rows, columns);
	double ** matriz = crear_mirror(rows, columns);
	archivo = open(nombre);
	for(int rindex = 0; rindex < rows; ++index){
		scanf(%f, matriz[rindex][cindex], archivo);
	}
	return matriz;

void search_clouds(double ** image, double ** mirror, int my_start, int my_finish, coordenadas, array_coordenadas, global_rows){
	for (int rows = 0; rows < global_rows; ++rows){
		for (int col = my_start; col < my_finish; ++col){
			if(mirror[row][col] == 0){
				array_coordenadas[cloud_count].cloud_id = cloud_count;
				array_coordenadas[cloud_count].area 0;
				expand_cloud(row, col, array_coordenadas);
			}
			else
				mirror[row][col] = -1;
		}
	}
}


void expand_cloud(int row, int col, coordenadas* array_coordenadas){
	if !(exist(rows, col))
		return;
	if mirror[rows][col] != 0
		return;
	if (a <= image[row][col] && b >= image[row][col]){
		mirror[row][col] = array[cloud_count].cloud_id;
		array[cloud_count].area++;
		expand_cloud
		expand_cloud
		expand_cloud
		expand_cloud
	}
	else
		mirror[row][col] = -1;
}

print_clouds(coordenadas* coordenadas){
	for(int index = 0; index < size(coordenadas); index++){
		printf(%d: %d, index++, coordenadas[index].area);
}

bool exist(row, col);
	return row >= 0 && row < global_rows && col >= 0 && col <= global_col;