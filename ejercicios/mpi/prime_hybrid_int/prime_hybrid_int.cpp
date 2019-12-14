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

bool is_prime(size_t number)
{
    if ( number < 2 ) return false;
    if ( number == 2 ) return true;
    if ( number % 2 == 0 ) return false;

    for ( size_t i = 3, last = (size_t)(double)sqrt(number); i <= last; i += 2 )
        if ( number % i == 0 )
            return false;

    return true;
}

int calculate_start(int worker_id, int workers, int finish, int begin)
{
	int range = finish - begin;
	return begin + worker_id * (range / workers) + std::min(worker_id, range % workers);
}

int calculate_finish(int worker_id, int workers, int finish, int begin)
{
	return calculate_start(worker_id + 1, workers, finish, begin);
}

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int my_rank = -1;
	int process_count = -1;
    size_t prime_count = 0;
    size_t local_prime_count = 0;


	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	char hostname[MPI_MAX_PROCESSOR_NAME];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);

	int global_start = 0;
	int global_finish = 0;
	int global_thread_count = 0;
	double start_time = MPI_Wtime();

	int thread_count = omp_get_max_threads();

	if ( argc >= 3 )
	{
		global_start = atoi(argv[1]);
		global_finish = atoi(argv[2]);
	}
	else
	{
		if ( my_rank == 0 )
		{
			std::cin >> global_start >> global_finish;
			for ( int destination = 1; destination < process_count; ++destination )
			{
				MPI_Send(&global_start, 1, MPI_INT, destination, /*tag*/ 0, MPI_COMM_WORLD);
				MPI_Send(&global_finish, 1, MPI_INT, destination, /*tag*/ 0, MPI_COMM_WORLD);
			}
		}
		else
		{
			MPI_Recv(&global_start, 1, MPI_INT, /*source*/ 0, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&global_finish, 1, MPI_INT, /*source*/ 0, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}

	const int my_start = calculate_start( my_rank, process_count, global_finish, global_start);
	const int my_finish = calculate_finish( my_rank, process_count, global_finish, global_start);


	#pragma omp parallel num_threads(thread_count) default(none) shared(my_rank, hostname, local_prime_count, std::cout)
	{
		#pragma omp for reduction(+:local_prime_count)
		for ( int index = my_start; index <= my_finish; ++index )
        	if ( is_prime(index) )
            	++local_prime_count;
	}
	
	MPI_Reduce(&local_prime_count, &prime_count, /*count*/ 1, MPI_INT, MPI_SUM, /*root*/ 0, MPI_COMM_WORLD);
	MPI_Reduce(&thread_count, &global_thread_count, /*count*/ 1, MPI_INT, MPI_SUM, /*root*/ 0, MPI_COMM_WORLD);

	double elapsed = MPI_Wtime() - start_time;

	if ( my_rank == 0 )
		std::cout << prime_count << " primes found in range [" << global_start << ", " << global_finish << 
		"[ in " << std::fixed << std::setprecision(9) << elapsed << 's' << 
		" with " << global_thread_count << std::endl;

	MPI_Finalize();
}