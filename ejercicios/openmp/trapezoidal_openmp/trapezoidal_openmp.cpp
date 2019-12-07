#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <cstdlib>
#include <iostream>

double positive_function(double preimage);
double trapezoidal_area(double (*function)(double), double lower_bound, double upper_bound, int division_quantity, int thread_count);
int main (int argc, char* argv[])
{
    double area = 0;
    double lower_bound = 0;
    double upper_bound = 0;
    int divisions = 0;
    int thread_count = omp_get_max_threads();

    if(argc < 4){
        return (void)fprintf(stderr,"not enough arguments\n"), 1;
    }

    if ((double) atof(argv[1]) < 0){
        return (void)fprintf(stderr,"The lower bound can't be a negative number\n"), 1;
    }

    if ((double) atof(argv[1]) > (atof)(argv[2])){
        return (void)fprintf(stderr,"The lower bound can't be greater than the upper bound\n"), 1;
    }

    if ((double)atof(argv[3]) == 0.0){
        return (void)fprintf(stderr,"The amount of division can't be  0\n"), 1;
    }

    if(argc >= 5)
    	thread_count = atoi(argv[4]);


    lower_bound = atof(argv[1]);
    upper_bound = atof(argv[2]);
    divisions = atoi(argv[3]);
/*
    Inicia el conteo del tiempo de creación
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
*/
    	area = trapezoidal_area(positive_function, lower_bound, upper_bound, divisions, thread_count);

    //Se determina el tiempo de creación
    struct timespec finish_time;
    clock_gettime(CLOCK_MONOTONIC, &finish_time);
    ///printf("Hello world from main thread\n");
/*
    double elapsed_seconds = finish_time.tv_sec - start_time.tv_sec
        + 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);
*/
    std::cout << "El area es: " << area << std::endl;


    return 0;
}


double positive_function(double x){
    double image = sqrt((x*(pow(x,2) - 6*x + 9))/9);
    return image;
}

double trapezoidal_area(double (*function)(double), double lower_bound, double upper_bound, int division_quantity, int thread_count){
    double area = 0;
    double jump = ((upper_bound-lower_bound)/division_quantity);
    #pragma omp parallel num_threads(thread_count) \
		default(none) shared(lower_bound, upper_bound, division_quantity, function, area, jump, std::cout)
	{
		double local_area = 0.0;
		double local_lower_bound = 0.0;
		#pragma omp for
		for(int iteration = 0; iteration < division_quantity; iteration++)
		{
			#pragma omp critical(lower_bound)
			{
				local_lower_bound = lower_bound;
				lower_bound += jump;
			}	
			local_area += (function(local_lower_bound) + function(local_lower_bound+jump));
		}
		#pragma omp critical(cout)
			area += local_area;
	}
    area *= (jump/2);
    return area;
}
