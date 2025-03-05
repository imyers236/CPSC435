/* File: matMul.c
* Author: Ian Myers
* Purpose: Implement matrix-matrix multiplication using
* one-dimensional arrays to store the matrices and pthreads.
*
* Compile: gcc –o pth_matMul pth_matMul.c -lpthread
* Run: use script file
*
* Input: Dimensions of the matrix: nxn
* Output: Result matC = matA x MatB
*
* Notes:
* 1. If the n cannot be evenly divided by the number of pthreads,
* the last process (size-1) takes care of the extra rows.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

void *Pth_mat_mul(void* rank);

/* Global variable: accessible ot all threads*/
int nthreads;
int n; // dimension of matrix
double *matA, *matB, *matC; // matrices

int main(int argc, char *argv[])
{
    long thread; /* Use long in case of a 64-bit system */
    struct timeval start, end;
    double cpu_time_used;
    pthread_t* thread_handles;
    n = atoi(argv[1]); // get matrix dimension from command line
    nthreads = strtol(argv[2], NULL, 10); // get amount of threads
    
    // adds extra rows to final process
    matB = (double *)calloc(n*n, sizeof(double)); /* allocate memory for matB */
    matA = (double *)calloc(n*n, sizeof(double)); /* allocate memory for
    matA */
    matC = (double *)calloc(n*n, sizeof(double)); /* allocate memory for
    matC */
    // Sets values for all matrices
    for(int i=0; i<n*n; i++)
    {
        matA[i] = 1.0;
        matB[i] = 2.0;
        matC[i] = 0.0;
    }
    // allocate memory for threads
    thread_handles = malloc (nthreads*sizeof(pthread_t));
    // start clock
    gettimeofday(&start, NULL);
    // assign threads to function
    for (thread = 0; thread < nthreads; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, Pth_mat_mul, (void*) thread);
    }
    // join threads together
    for (thread = 0; thread< nthreads; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }
    // end clock
    gettimeofday(&end, NULL);
    // calulate computation time
    cpu_time_used = (end.tv_sec - start.tv_sec) * 1e6;
    cpu_time_used = (cpu_time_used + (end.tv_usec - start.tv_usec)) * 1e-6;

    // print results
    printf("Total computation time is %f seconds.\n", cpu_time_used);
    if(n <= 40){
        printf("Result C :\n");
        for (int i=0; i<n*n; i++) {
            if (i%n == 0) printf("\n");
            printf("%6.2f\t", matC[i]);
        }
    }
    printf("\n");
    // free memory
    free(matA);
    free(matB);
    free(matC);
    free(thread_handles);
    return 0;
}

void *Pth_mat_mul(void* rank)
{
    // finds rank
    long my_rank = (long) rank;
    int i, j, k;
    int extraRows, rowsperthread;
    int my_last_row;
    // finds rows per thread
    rowsperthread = n / nthreads;
    int my_first_row = my_rank*rowsperthread;
    // checks if a thread must do more rows
    if(my_rank == nthreads-1){
        extraRows = n - rowsperthread * nthreads;
        my_last_row = (my_rank+1)*rowsperthread - 1 + extraRows;
    }
    else
    {
        my_last_row = (my_rank+1)*rowsperthread - 1;
    }
    // does the matrix multiplication
    for(i=my_first_row; i<= my_last_row; i++)
    {
        for(j=0; j<n; j++)
        {
            for(k=0; k<n; k++)
            {
                matC[i*n + j] += matA[i*n + k]*matB[k*n + j];
            }
        }
    }
    
    return NULL;
}