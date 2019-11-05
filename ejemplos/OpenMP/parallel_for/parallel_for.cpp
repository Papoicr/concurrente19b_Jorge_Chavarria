#include <cstdlib>
#include <iostream>
#include <omp.h>

int main (int argc, char* argv[])
{
	int thread_count = omp_get_max_threads();
	if(argc >= 2)
		thread_count = atoi(argv[1]);
		
	int iteration_count = thread_count;
	if(argc >= 2)
		iteration_count = atoi(argv[2]);
	
	#pragma omp parallel for num_threads(thread_count) \
		default(none) shared(iteration_count, std::cout) /// El pragma es una instruccion para el compilador específico y los de openMP llevan omp
	for (int iteration = 0; iteration < iteration_count; ++iteration)
	{
		#pragma omp critical(cout)
		std::cout << omp_get_thread_num () << " / " << omp_get_num_threads() << ": iteration " << iteration << " / " << iteration_count << std::endl;
	}
	return 0;	
}
