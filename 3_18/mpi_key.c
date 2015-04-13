/*
 * mpi_college.c
 * Author: Christian Würthner
 * Description: Homework Cahpter 4 Excercise 10
 */

#define LIST_SIZE 65536

#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <inttypes.h>

 int main(int argc, char **argv) {
 	int32_t process_count, rank;

	/* MPI init */
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* Generate list */
	uint32_t a[LIST_SIZE] = {0};

	/* Calculate range */
 	uint32_t start = (LIST_SIZE / process_count) * rank + 
		((LIST_SIZE % process_count) < rank ? (LIST_SIZE % process_count) : rank);
	uint32_t end = start + (LIST_SIZE / process_count) + 
		((LIST_SIZE % process_count) > rank) - 1;

	uint32_t largest_keys[2] = {0};

	/* Iterate over range and ssearch largest and second largest key */
	for (uint32_t i=start; i<=end; i++) {
		if(largest_keys[1] < a[i]) {
			largest_keys[1] = a[i];
		}
		if(largest_keys[0] < a[i]) {
			largest_keys[1] = largest_keys[0];
			largest_keys[0] = a[i];
		}

	}

	printf("Keys in range [%d, %d] found: %d > %d\n", 
		start, end, largest_keys[0], largest_keys[1]);

	/* Code for worker process */
	if (rank == 0) {
		uint32_t remote_keys;
		uint32_t keys[process_count*2];

		/* Wait for process_count-1 processes to finish and add their results to
		   local sum */
		for (int source = 1; source < process_count; source++) {
			MPI_Recv(&remote_keys, 1, MPI_INTEGER8, source, 0, 
				MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			keys[source] = *((uint32_t *) &remote_keys);
			keys[source+1] = *(((uint32_t *) &remote_keys) + 1);

			printf("Keys received: %d > %d\n", keys[source], keys[source+1]);
		}
	} 

	/* Code for worker process */
	else {
		/* Send result to master (process 0) */
		MPI_Send(largest_keys, 1, MPI_INTEGER8, 0, 0, MPI_COMM_WORLD);
	}

	/* MPI finalize */
	MPI_Finalize();

	return 0;

}