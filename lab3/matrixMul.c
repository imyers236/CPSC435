/* File: matMul.c
* Author: Ian Myers
* Purpose: Implement parallel matrix-matrix multiplication using
* one-dimensional arrays to store the matrices.
* Matrix A & B are distributed to slaves through MPI_Send.
*
* Compile: mpicc -g -Wall -o matMul matMul.c
* Run: use script file
*
* Input: Dimensions of the matrix: nxn
* Output: Result matC = matA x MatB
*
* Notes:
* 1. If the n cannot be evenly divided by the number of processes,
* the last process (size-1) takes care of the extra rows.
*/
#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#define TAG_INITA 1 // Tag for sending/receiving Matrix A
#define TAG_INITB 2 // Tag for sending/receiving Matrix B
#define TAG_RESULT 3 // Tag for collecting result Matrix C
int main(int argc, char *argv[])
{
    double *matA, *matB, *matC;
    int size, rank;
    int extraRows, rowsperprocess;
    int i, j, k;
    double t;
    MPI_Status status;
    int n; // dimension of matrix
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    n = atoi(argv[1]); // get matrix dimension from command line
    rowsperprocess = n / (size - 1);
    extraRows = n - rowsperprocess * (size -1);
    if(rank == size-1){
        rowsperprocess += extraRows;
    }
    matB = (double *)calloc(n*n, sizeof(double)); /* allocate memory for matB */
    if(rank == 0) // master: allocate memory, initialize Matricies, send matB and chuck of matA
    {
        matA = (double *)calloc(n*n, sizeof(double)); /* allocate memory for
        matA */
        matC = (double *)calloc(n*n, sizeof(double)); /* allocate memory for
        matC */
        // Sets values for all matrices
        for(i=0; i<n*n; i++)
        {
            matA[i] = 1.0;
            matB[i] = 2.0;
            matC[i] = 0.0;
        }
        t = MPI_Wtime(); //start timer
        for(i=1; i<size; i++) // send marix B and chunk of A to each slave
        {
            MPI_Send(&matA[(rank -1) * rowsperprocess], n, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
            MPI_Send(matB, n*n, MPI_DOUBLE, i, 2, MPI_COMM_WORLD);
        }
    }
    else //slaves: allocate memory for matrix A and C, and receive matrix B and A
    {
        matA = (double *)calloc(n*n, sizeof(double)); /* allocate memory for
        matA */
        matC = (double *)calloc(n*n, sizeof(double)); /* allocate memory for
        matC */
        MPI_Recv(&matA, n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(matB, n*n, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &status);

    }
    if(rank >0) // slaves: do computation and send results to master
    {
        // each slave does the multiplication of received portion
        for(i=(rank -1) * rowsperprocess; i< rank * rowsperprocess; i++)
        {
            for(j=0; j<n; j++)
            {
                for(k=0; k<n; k++)
                {
                    matC[i*n + j] += matA[i*n + k]*matB[k*n + j];
                }
            }
        }    
        MPI_Send(matC, n*n, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);         
    }
    else // master receive result, record time and print out results if n<16
    {
        for(i=1; i<size; i++) // send marix B and chunk of A to each slave
        {
            MPI_Recv(matC, n*n, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
        }
        // Stop timer, including communication time
        t = MPI_Wtime() - t;
        printf("Total computation time is %f seconds.\n", t);
        if(n <= 16){
            printf("Result C :\n");
            for (i=0; i<n*n; i++) {
                if (i%n == 0) printf("\n");
                printf("%6.2f\t", matC[i]);
            }
        }
        printf("\n");
    }
    free(matA);
    free(matB);
    free(matC);
    MPI_Finalize();
}