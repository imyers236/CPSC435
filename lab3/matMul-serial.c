// Serial matrix multiplication

#include <stdio.h>
#include <stdlib.h>
#include "sys/time.h"

#define N 512


int main(int argc, char *argv[])
{
    double *matA, *matB, *matC;
    int i, j, k;
    struct timeval tpstart, tpend;
    float timeuse;

    matA = (double *)calloc(N*N, sizeof(double));   /* allocate memory for matA */
    matB = (double *)calloc(N*N, sizeof(double));   /* allocate memory for matB */
    matC = (double *)calloc(N*N, sizeof(double));   /* allocate memory for matC */
    for(i=0; i<N*N; i++)
    {
          matA[i] = 1.0;
          matB[i] = 2.0;
	  matC[i] = 0.0;
    }
    gettimeofday(&tpstart, NULL);
    for(i=0; i<N; i++)
    {
          for(j=0; j<N; j++)
               for(k=0; k<N; k++)
               {
                     matC[i*N + j] += matA[i*N + k]*matB[k*N + j]; 
               }
    }

    gettimeofday(&tpend, NULL);
    timeuse = (tpend.tv_sec - tpstart.tv_sec)+(tpend.tv_usec - tpstart.tv_usec)/(float)1000000;
    printf("Used Time (s): %f \n", timeuse);

    if(N <= 16){
         printf("Result C :\n");
         for (i=0; i<N*N; i++) {
             if (i%N == 0) printf("\n");
             printf("%6.2f\t", matC[i]);
	 }
    }
    printf("\n"); 


    free(matA);
    free(matB);
    free(matC);

}

