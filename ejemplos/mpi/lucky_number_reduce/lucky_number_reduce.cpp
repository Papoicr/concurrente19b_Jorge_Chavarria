#include <mpi.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <cstring>

int main (int argc, char *argv[])
{
	/*int MPI_Reduce(const void *sendbuf, void *recvbuf, int count,
               MPI_Datatype datatype, MPI_Op op, int root,
               MPI_Comm comm)
	*/
	MPI_Init(&argc, &argv);
	
	int my_rank = -1;
	int process_count = -1;
	
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	
	char hostname [MPI_MAX_PROCESSOR_NAME];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);
	
	srand(my_rank + time(nullptr) + clock());
	int my_lucky_number = rand() % 100;
	
	
	
	int global_min = -1;
	int global_max = -1;
	int global_sum = 0.0;
	
	MPI_Reduce(&my_lucky_number, &global_min , /**count*/ 1, MPI_INT, MPI_MIN, /**ROOT*/ 0, MPI_COMM_WORLD);
	MPI_Reduce(&my_lucky_number, &global_max , /**count*/ 1, MPI_INT, MPI_MAX, /**ROOT*/ 0, MPI_COMM_WORLD);
	MPI_Reduce(&my_lucky_number, &global_sum , /**count*/ 1, MPI_INT, MPI_SUM, /**ROOT*/ 0, MPI_COMM_WORLD);


	char next = 'a';
	
	if(my_rank != 0)
		MPI_Recv(&next, 1, MPI_CHAR, my_rank-1, 0, MPI_COMM_WORLD,  MPI_STATUS_IGNORE);
		
	std::cout << "Process " << my_rank << ": my lucky number is " << std::setw(2) << std::setfill('0') << my_lucky_number << std::endl;
	
	if(my_rank == 0){
		double average = static_cast<double>(global_sum) / process_count;
		std::cout << "Process " << my_rank << ": all minimun: " << std::setw(2) << std::setfill('0') << global_min << std::endl;
		std::cout << "Process " << my_rank << ": all average: " << std::fixed << std::setprecision(2) << average << std::endl;
		std::cout << "Process " << my_rank << ": all maximun: " << std::setw(2) << std::setfill('0') << global_max << std::endl;
	}

	if(my_rank < process_count -1)
		MPI_Send(&next, 1, MPI_CHAR, my_rank+1, 0, MPI_COMM_WORLD);
	
	MPI_Finalize();
}
