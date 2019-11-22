#include <mpi.h>
#include <iostream>
#include <cstdio>
#include <cstring>

#define MESSAGE_CAPACITY 1024

int main (int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	
	int my_rank = -1;
	int process_count = -1;
	
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	
	char hostname [MPI_MAX_PROCESSOR_NAME];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);
	
	char message[1024];
	sprintf(message, "Hello from main thread of process %d of %d on %s\n", my_rank+1, process_count, hostname); ///sprintf deja todo en un archivo 
	//std::cout << "Hello from main thread" << my_rank << " of " << process_count << " on " << hostname << //std::endl;
	
	if(my_rank == 0)
	{
		std::cout << message; 	
		for(int sender = 1; sender < process_count; ++sender){
			MPI_Recv(message, /*capacity*/ MESSAGE_CAPACITY, MPI_CHAR, /*source*/ sender, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			std::cout << message;
		}
	}
	else
	{
		MPI_Send(message, /*count*/ strlen(message) + 1, MPI_CHAR, /*dest*/ 0, /*tag*/ 0 , MPI_COMM_WORLD);
	}
	MPI_Finalize();
}


 ///Lo que se envia tiene que estar en la memoria, no se puede enviar el resultado. (si se quiere el resultado se tiene que guardar en una variable de memoria)
