#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int my_rank = -1;
	int process_count = -1;
	int hot_potato = 0;
	int perdi = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	char hostname[MPI_MAX_PROCESSOR_NAME];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);

	if ( argc >= 2 )
	{
		hot_potato = atoi(argv[1]);
	}
	else
	{
		if ( my_rank == 0 )
			std::cin >> hot_potato;
	}

	if ( my_rank == 0 ){
		--hot_potato;
		if(hot_potato < 0){
			std::cout << my_rank << " perdi " << std::endl;
			perdi = 1;
		}
		MPI_Send(&hot_potato, /*count*/ 1, MPI_INT, /*dest*/ (my_rank + 1), /*tag*/ 0, MPI_COMM_WORLD);
	}

	while ( hot_potato >= 0){
		if ( my_rank != 0 ){
			MPI_Recv(&hot_potato, /*capacity*/ 1, MPI_INT, /*source*/ my_rank - 1, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else{
			MPI_Recv(&hot_potato, /*capacity*/ 1, MPI_INT, /*source*/ process_count-1, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		if(hot_potato >= 0){
			--hot_potato;
			if(hot_potato < 0){
				std::cout << my_rank << " perdi " << std::endl; 
			}
		}
		MPI_Send(&hot_potato, /*count*/ 1, MPI_INT, /*dest*/ (my_rank + 1)%(process_count), /*tag*/ 0, MPI_COMM_WORLD);
	}

	if(perdi == 1)
		MPI_Send(&hot_potato, /*capacity*/ 1, MPI_INT, /*source*/ (my_rank + 1)%(process_count), /*tag*/ 0, MPI_COMM_WORLD);

	MPI_Finalize();
}