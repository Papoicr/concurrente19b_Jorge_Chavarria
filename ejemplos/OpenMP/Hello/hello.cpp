#include <iostream>

int main (/*int argc, char* argv[]*/)
{
	#pragma omp parallel /// El pragma es una instruccion para el compilador específico y los de openMP llevan omp
	{
		//std::cout << "Hello world from secondary thread" << thread_num << "of" << thread_count << std::endl;
		#pragma omp critical(cout)
		std::cout << "Hello world from secondary thread" << std::endl;
	}
	return 0;	
}
