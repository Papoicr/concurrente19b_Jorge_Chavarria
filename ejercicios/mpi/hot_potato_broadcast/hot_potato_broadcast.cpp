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

	int start = 0;
	int my_rank = -1;
	int process_count = -1;
	int potato_value = 0;
	int hot_potato = 0;
	int perdi = 0;
	int next = 0;
	int temp = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	char hostname[MPI_MAX_PROCESSOR_NAME];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);

	int jugadores = process_count;

	if ( argc >= 3 )
	{
		potato_value = atoi(argv[1]);
		start = atoi(argv[2]);
	}

	hot_potato = potato_value;

	if(hot_potato % 2 != 0)
		hot_potato = 3*hot_potato+1;
	else
		hot_potato /= 2;

	if(hot_potato == 1){
		--jugadores;
		perdi = 1;
		hot_potato = potato_value;
	}
	next = (my_rank + 1)%(process_count);
	MPI_Bcast(&hot_potato, 1, MPI_INT, /*root*/(start)%(process_count), MPI_COMM_WORLD);
	MPI_Bcast(&jugadores, 1, MPI_INT, /*root*/(start)%(process_count), MPI_COMM_WORLD);
	MPI_Bcast(&next, 1, MPI_INT, /*root*/(start)%(process_count), MPI_COMM_WORLD);


	while ( hot_potato >= 0 && jugadores > 1){
		if(my_rank == next){
			if(hot_potato >= 1 && perdi == 0 && jugadores > 1){
				if(hot_potato % 2 != 0)
					hot_potato = 3*hot_potato+1;
				else
					hot_potato /= 2;
				
				if(hot_potato == 1){
					--jugadores;
					perdi = 1;
					hot_potato = potato_value;
				}
			}
		}
		temp = next;
		MPI_Bcast(&hot_potato, 1, MPI_INT, /*tag*/ next, MPI_COMM_WORLD);
		MPI_Bcast(&jugadores, 1, MPI_INT, /*tag*/ next, MPI_COMM_WORLD);
		next = (my_rank + 1)%(process_count);
		MPI_Bcast(&next, 1, MPI_INT, /*tag*/ temp, MPI_COMM_WORLD);
	}

	if(perdi == 0){
		std::cout << "Process " << my_rank << " gano " << std::endl;
	}

	MPI_Finalize();
}