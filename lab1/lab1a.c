/*
Ian Myers
CPSC 435
01/27/25
desc: Creates a matrix then adds up float numbers in each row of the nxn
matrix using column-wise memory access. The result is saved in the
vector
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    // initialize n for matrix
    int n = 5000;

    // initialize time variables
    clock_t start, end;
    double cpu_time_used;

    // Allocate memory for the matrix
    float **matrix = (float **)malloc(n * sizeof(float *));
    for (int i = 0; i < n; i++) {
        matrix[i] = (float *)malloc(n * sizeof(float));
    }

    // Allocate memory for the vector
    float *vector = (float *)malloc(n * sizeof(float));

    // Initialize the matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = 1;
        }
    }

    // Initialize the vector
    for (int i = 0; i < n; i++) {
            vector[i] = 0;
    }

    // start timer
    start = clock();

    // Add the matrix to the vector by column order
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            vector[j] += matrix[j][i];
        }
    }

    // end timer and calculate time
    end = clock();
    cpu_time_used = ((double) (end - start));

    // print vector and time taken
    for (int i = 0; i < n; i++) {
            printf("Index: %d, Value: %f\n", i,vector[i]);
    }
    printf("Time taken: %f microseconds\n", cpu_time_used);

    // Deallocate memory
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(vector);

    return 0;
}