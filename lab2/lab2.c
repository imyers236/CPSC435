/*
* Ian Myers
* All processes find out the smallest process ID and print out their results
* after exchanging all pids.
*/
#include <stdio.h>
#include <string.h> /* For strlen */
#include <mpi.h> /* For MPI functions, etc */
#include <sys/types.h> // to get the pids
#include <unistd.h>

int main(void) {
    int comm_sz; /* Number of processes */
    int my_rank; /* My process rank */
    int lowest_proccess;
    pid_t pid;  /* My processes pid*/
    pid_t other_pid; /* the other process pid*/
    pid_t lowest_pid; /* the lowest process pid */
    pid = getpid(); // get the process id
    lowest_pid = pid; // sets the initial smallest to pid
    MPI_Init(NULL, NULL); /* Start up MPI */
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); /* Get the number of processes */
    /* Get my rank among all the processes */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    lowest_proccess = my_rank; // set initial smallest to my_rank
    for (int i = 0; i < comm_sz; i++) {
            /* If not my rank swap pid's*/
            if (i != my_rank) {
                MPI_Send(&pid, 1, MPI_INT, i, 0, MPI_COMM_WORLD); /* Send PID to i */

                /* Receive message from process i */
                MPI_Recv(&other_pid, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                /* Checks if there is a new lowest*/
                if(other_pid < lowest_pid)
                {
                    lowest_pid = other_pid;
                    lowest_proccess = i;
                }
            }
    }
    /* Print message */
    printf("Process %d: my ID is %d; the smallest process ID %d is the one from Process %d\n", my_rank, pid, lowest_pid, lowest_proccess);
    MPI_Finalize(); /* Shut down MPI */
    return 0;
} /* main */