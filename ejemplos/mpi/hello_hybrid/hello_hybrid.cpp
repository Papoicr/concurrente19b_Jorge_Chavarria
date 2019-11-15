#include <omp.h>
#include <mpi.h>
#include <iostream>

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
	
	std::cout << "Hello from main thread of process" << my_rank << " of " << process_count << " on " << hostname << std::endl;
	
	#pragma omp parallel default(none) shared(my_rank, hostname, std::cout)
	{	
		#pragma omp critical(stdcout)
		{
			std::cout << "	Hello from main thread " << omp_get_thread_num() << " of " << omp_get_num_threads() << " of process " << my_rank << " on " << hostname << std::endl;
		}
	}
	MPI_Finalize();
}
