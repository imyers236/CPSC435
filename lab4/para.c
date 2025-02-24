/*
* Author: Ian Myers
* program: parallel version with MPI using odd-even sorting
*/

#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
void quick_sort( int *array, int lo, int hi, int flag);
void bubbleSort(int numbers[], int array_size);
void CompareSplit(int nlocal, int *elmnts, int *relmnts, int *wspace, int
keepsmall);
int main(int argc, char *argv[])
{
    int *totalElmnts; // the array that stores all the elements
    int *elmnts, *relmnts; // local elements
    int npes, myrank;
    int n, nlocal;
    int oddrank, evenrank;
    int *wspace;
    int i;
    double t;
    MPI_Status status;
    //initialize MPI environment
    MPI_Init(&argc, &argv);
    // get myrank
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    // get npes
    MPI_Comm_size(MPI_COMM_WORLD, &npes);
    n = atoi(argv[1]);
    nlocal = n/npes;
    /* allocate memory for local elments (i.e., elmnts) */
    elmnts = (int *)malloc(nlocal*sizeof(int));
    /* allocate memory for receving elements (i.e.,
    relmnts)*/
    relmnts = (int *)malloc(nlocal*sizeof(int));
    /* allocate memory for receving elements (i.e.,
    wspace)*/
    wspace = (int *)malloc(nlocal*sizeof(int));
    if(myrank == 0) /* master process allocates memory for global sequence
    (i.e., totalElmnts) and then initialize it with random numbers */
    {
        srand(MPI_Wtime());   // Initialization for random numbers
        totalElmnts = (int *)malloc(n*sizeof(int));
        // Sets random values for totalElments
        for(i=0; i<n; i++)
        {
            totalElmnts[i] = rand() % (n + 1);
        }
        t = MPI_Wtime(); //start timer
    }
    // distribute one portion of totalelmnts to each process using a collective function
    MPI_Scatter(totalElmnts, nlocal, MPI_INT, elmnts, nlocal, MPI_INT, 0, MPI_COMM_WORLD);
    // sort the local elements using bubbleSort or quick_sort
    bubbleSort(elmnts, nlocal);
    //quick_sort(elmnts, 0, nlocal-1, 1);
    /* determine the rank of processors that myrank needs to communicate during the
    odd and even phases of the algorithm */
    if (myrank % 2 == 0)
    {
        oddrank = myrank - 1;
        evenrank = myrank + 1;
    }
    else
    {
        oddrank = myrank + 1;
        evenrank = myrank - 1;
    }
    // edge cases for odd and even ranks
    if(oddrank == -1 || oddrank == npes)
    {
        oddrank = MPI_PROC_NULL;
    }
    if(evenrank == -1 || evenrank == npes)
    {
        evenrank = MPI_PROC_NULL;
    }
    /* set the ranks of the processors at the end of the linear */
    /* get into the main loop of the odd-even sorting algorithm */
    for(i=0; i<npes; i++)
    {
        if(i%2 == 1) 
        {
            MPI_Sendrecv(elmnts, nlocal, MPI_INT, oddrank, 1, relmnts, nlocal, MPI_INT, oddrank, 1, MPI_COMM_WORLD, &status);
            if(myrank!=0 && myrank!=npes-1)
            {
                CompareSplit(nlocal, elmnts, relmnts, wspace, myrank<status.MPI_SOURCE);
            }     
        }
        else
        {
            MPI_Sendrecv(elmnts, nlocal, MPI_INT, evenrank, 1, relmnts,nlocal, MPI_INT, evenrank, 1, MPI_COMM_WORLD, &status);
            CompareSplit(nlocal, elmnts, relmnts, wspace, myrank<status.MPI_SOURCE);
        }  
    }
    // gather results and put it the totalElmnts in the master process using collective opertaion
    MPI_Gather(elmnts, nlocal, MPI_INT, totalElmnts, nlocal, MPI_INT, 0, MPI_COMM_WORLD);
    if(myrank == 0) // master process prints out sorted results if n<48
    {
        // master process records stop timer, print out computation time, and free totalElmnts
        t = MPI_Wtime() - t;
        printf("Total computation time is %f seconds.\n", t);
        if(n <= 48){
            printf("Result:\n");
            for (i=0; i<n; i++) {
                printf("%d\t", totalElmnts[i]);
            }
        }
        printf("\n");
        free(totalElmnts);
    }
    free(elmnts);
    free(relmnts);
    free(wspace);
    MPI_Finalize();
}
void CompareSplit(int nlocal, int *elmnts, int *relmnts, int *wspace, int keepsmall)
{
    int i, j, k;
    for(i=0; i<nlocal; i++)
        wspace[i] = elmnts[i];
    if (keepsmall)
    {
        for(i=j=k=0; k<nlocal; k++)
        {
            if(j==nlocal || (i < nlocal && wspace[i] < relmnts[j]))
                elmnts[k] = wspace[i++];
            else
                elmnts[k] = relmnts[j++];
        }
    }
    else
    {
        for(i=k=nlocal-1, j=nlocal-1; k>=0; k--)
        {
            if(j==-1 || (i >= 0 && wspace[i] >= relmnts[j]))
                elmnts[k] = wspace[i--];
            else
                elmnts[k] = relmnts[j--];
        }
    }
}
void quick_sort(int *array, int lo, int hi, int flag)
{
    int i = lo-1;
    int j = hi;
    int pivot= array[hi];
    int temp;
    if (hi>lo)
    {
        do
        {
            if(flag == 1) /* a increase sort */
            {
            do i++; while (array[i]<pivot);
            do j--; while (array[j]>pivot);
            }
            else /* a decrease sort */
            {
            do i++; while (array[i]>pivot);
            do j--; while (array[j]<pivot);
            }
            temp = array[i]; /* swap values */
            array[i] = array[j];
            array[j] = temp;
        } while (j>i);
        array[j] = array[i]; /* swap values */
        array[i] = pivot;
        array[hi] = temp;
        quick_sort(array,lo,i-1,flag); /* recursive until hi == lo */
        quick_sort(array,i+1,hi,flag);
    }
}
void bubbleSort(int numbers[], int array_size)
{
    int i, j, temp;
    for (i = (array_size - 1); i > 0; i--)
    {
        for (j = 1; j <= i; j++)
        {
            if (numbers[j-1] > numbers[j])
            {
                temp = numbers[j-1];
                numbers[j-1] = numbers[j];
                numbers[j] = temp;
            }
        }
    }
}