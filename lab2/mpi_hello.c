/*
* MPI Example - Hello world
*/
#include <stdio.h>
#include <string.h> /* For strlen */
#include <mpi.h> /* For MPI functions, etc */
const int MAX_STRING = 100;
int main(void) {
char greeting[MAX_STRING]; /* String storing message */
int comm_sz; /* Number of processes */
int my_rank; /* My process rank */
MPI_Init(NULL, NULL); /* Start up MPI */
MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); /* Get the number of processes */
/* Get my rank among all the processes */
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
if (my_rank != 0) {
sprintf(greeting, "Greetings from process %d of %d!",
my_rank, comm_sz); /* Create message */
MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 0, 0,
MPI_COMM_WORLD); /* Send message to process 0 */
} else {
printf("Greetings from process %d of %d!\n", my_rank, comm_sz); /* Print my message */
for (int q = 1; q < comm_sz; q++) {
/* Receive message from process q */
MPI_Recv(greeting, MAX_STRING, MPI_CHAR, q,
0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
printf("%s\n", greeting); /* Print message from process q */
}
}
MPI_Finalize(); /* Shut down MPI */
return 0;
} /* main */
