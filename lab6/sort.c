/* File: sort.c
* Author: Ian Myers
* Purpose: Implement array sorting function using pthreads.
* Compile: gcc -o sort sort.c seq.c -lpthread
* Run: ./sort n nthreads
* Notes:
* 1. n must be evenly divided by the number of pthreads
*/
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#define MAX_THREADS 512
extern void quick_sort( int *array, int lo, int hi, int flag);
void CompareSplit(int nlocal, int *elmnts, int *relmnts, int *wspace, int
keepsmall);
typedef struct
{
    pthread_mutex_t count_lock;
    pthread_cond_t ok_to_proceed;
    int count;
}
mylib_barrier_t;
int *totalElmnts; // the array that stores all the elements
int n, nlocal;
static int nthreads;
mylib_barrier_t b;
void mylib_init_barrier(mylib_barrier_t *b)
{
    b -> count = 0;
    pthread_mutex_init(&(b->count_lock), NULL);
    pthread_cond_init(&(b->ok_to_proceed), NULL);
}
void mylib_barrier(mylib_barrier_t *b, int num_threads)
{
    pthread_mutex_lock(&(b->count_lock));
    b->count ++;
    if(b->count == num_threads)
    {
        b->count = 0;
        pthread_cond_broadcast(&(b->ok_to_proceed));
    }
    else
        while(pthread_cond_wait(&(b->ok_to_proceed), &(b->count_lock)) != 0);
            pthread_mutex_unlock(&(b->count_lock));
}
void odd_even(int threadid)
{
    int i, j;
    int myid;
    int *relmnts; // local elements
    int *wspace;
    myid = threadid;
    relmnts = (int *)calloc(nlocal, sizeof(int)); /* allocate memory for receving
    elements */
    wspace = (int *)calloc(nlocal, sizeof(int));
    quick_sort(totalElmnts+nlocal*myid, 0, nlocal-1, 1); // local sort
    mylib_barrier(&b, nthreads);
    for(i=0; i<nthreads && nthreads!=1; i++)
    {
        if(i%2 == 1) //odd phase
        {
            if(myid!=0 && myid!=nthreads-1)
            {
                if(myid%2 == 1) // use a for loop to copy data from totalElmnts to relmnts - right neighbour
                {
                    for(j = 0; j<nlocal; j++)
                    {
                        relmnts[j] = totalElmnts[nlocal*(myid+1) + j];
                    }
                }
                else // use a for loop to copy data from totalElmnts to relmnts - left neighbour
                {
                    for(j = 0; j<nlocal; j++)
                    {
                        relmnts[j] = totalElmnts[nlocal*(myid-1) + j];
                    }
                }
            }
            mylib_barrier(&b, nthreads);
            if(myid!=0 && myid!=nthreads-1) // compare and split - local elements: totalElmnts+nlocal*myid
            {
                if(myid%2 == 1) 
                {
                    CompareSplit(nlocal, totalElmnts+nlocal*myid, relmnts, wspace, 1);     
                }
                else
                {
                    CompareSplit(nlocal, totalElmnts+nlocal*myid, relmnts, wspace, 0);
                }  
            }
        }
        else //even phase
        {
            if(myid%2 == 0) // use a for loop to copy data from totalElmnts to relmnts - right neighbour
            {
                for(j = 0; j<nlocal; j++)
                {
                    relmnts[j] = totalElmnts[nlocal*(myid+1) + j];
                }
            }
            else // use a for loop to copy data from totalElmnts to relmnts - left neighbour
            {
                for(j = 0; j<nlocal; j++)
                {
                    relmnts[j] = totalElmnts[nlocal*(myid-1) + j];
                }
            }
            mylib_barrier(&b, nthreads);
            // compare and split - local elements: totalElmnts+nlocal*myid;
                if(myid%2 == 0) 
                {
                    CompareSplit(nlocal, totalElmnts+nlocal*myid, relmnts, wspace, 1);     
                }
                else
                {
                    CompareSplit(nlocal, totalElmnts+nlocal*myid, relmnts, wspace, 0);
                } 
        }
        mylib_barrier(&b, nthreads);
    }
    free(relmnts);
    free(wspace);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int i, j, t, rc;
    pthread_t threads[MAX_THREADS];
    double time_start, time_end;
    struct timeval tv;
    n = atoi(argv[1]);
    nthreads = atoi(argv[2]);
    nlocal = n/nthreads;
    mylib_init_barrier(&b);
    totalElmnts = (int *)calloc(n, sizeof(int)); /* allocate memory for global
    sequence */
    // generate random numbers and initialize totalElmnts
    srand(time(NULL));   // Initialization for random numbers
    // Sets random values for totalElments
    for(i=0; i<n; i++)
    {
        totalElmnts[i] = rand() % (n + 1);
    }
    gettimeofday(&tv, NULL);
    time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
    // create multiple threads
    for (t = 0; t< nthreads; t++)
    {
        rc = pthread_create(&threads[t], NULL,(void *(*)(void *))odd_even, (void*)(size_t)t);
        // error handling for threads
        if (rc)
        {
            printf("Error: threads not created %d\n", rc);
            exit(-1);
        }
    }
    // join threads together
    for (t = 0; t< nthreads; t++)
    {
        pthread_join(threads[t], NULL);
    }
    // wait for completion of threads
    gettimeofday(&tv, NULL);
    time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
    printf("Computation time: %6f \n", time_end-time_start);
    if(n<=40)
    {
        printf("Sorted elements: \n");
        for (i=0; i<n; i++) {
            if (i%6 == 0) printf("\n");
                printf("%6d\t", totalElmnts[i]);
        }
        printf("\n");
    }
    free(totalElmnts);
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