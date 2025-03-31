/* File: omp_pi.c
* Author: Ian Myers
* Purpose: Computes pi using the “Monte Carlo” method and openMP.
*
* Compile: gcc -g -Wall -fopenmp -o omp_pi omp_pi.c my_rand.c
* Run: ./omp_pi nthreads n
*
* Input: nthreads: number of threads, n: number of tosses
* Output: Result: computational estimate of pi
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
#include "my_rand.h"

int main(int argc, char *argv[])
{
    struct timeval start, end;
    double cpu_time_used;
    int num_tosses = atoi(argv[2]);; // the total number of tosses
    long double num_in_circle = 0.0; // the number of tosses in the circle
    int thread_count = strtol(argv[1], NULL, 10); // get amount of threads

    // variables
    // index
    int i;
    // x and y cordinates and their combined distance
    double x, y, distance_squared;

    // start clock
    gettimeofday(&start, NULL);
    // assign threads for loop
#   pragma omp parallel for num_threads(thread_count) \
    default(none) private(x, y, distance_squared) reduction(+:num_in_circle)\
    shared(num_tosses)
    for(i=0; i< num_tosses; i++)
    {
        // assign the seed for each iteration
        // change from omp_get_thread_num() + 1 due to a repetition error
        unsigned seed = i + 1;
        x = my_drand(&seed);
        y = my_drand(&seed);
        distance_squared = x*x + y*y;
        if (distance_squared <=1)
        {
            num_in_circle++;
        }
    }
    // computation
    double pi_estimate = (num_in_circle/num_tosses) * 4.0;
    // end clock
    gettimeofday(&end, NULL);
    // calulate computation time
    cpu_time_used = (end.tv_sec - start.tv_sec) * 1e6;
    cpu_time_used = (cpu_time_used + (end.tv_usec - start.tv_usec)) * 1e-6;

    // print results
    printf("Computational estimate of pi is %f, with total computation time being %f seconds.\n", pi_estimate, cpu_time_used);
    printf("\n");
    return 0;
}
