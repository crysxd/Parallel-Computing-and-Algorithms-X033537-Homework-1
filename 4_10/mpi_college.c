/*
 * mpi_college.c
 * Author: Christian Würthner
 * Description: Homework Cahpter 4 Excercise 10
 */

#define START 	99999
#define END		999999
#define STEPS	(END - START)

#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <inttypes.h>

void get_digits(uint32_t number, uint8_t *digits) {
	/* Split number in digits and fill array backwards */
	for(int i=0; i<6; i++) {
		digits[5-i] = number % 10;
		number /= 10;
	}
}

bool is_valid_id(uint32_t number) {
	/* get digits */
	uint8_t digits[6];
	get_digits(number, digits);

	/* Criteria 1 is automatically fulfilled by START = 99999+1 */

	/* Check criteria 2: Two consecutive number must not be the same */
	for(uint8_t i=1; i<sizeof(digits); i++) {
		if(digits[i-1] == digits[i])
			return false;
	} 

	/* Check criteria 3: digit sum must not be 7, 11 or 13 */
	uint8_t sum = 0;
	for(uint8_t i=0; i<sizeof(digits); i++) {
		sum+=digits[i];
	}

	if(sum == 7 || sum == 13 || sum == 13) {
		return false;
	}

	return true;
}

int main(int argc, char **argv) {
 	int32_t process_count, rank;

	/* MPI init */
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* Calculate range */
 	uint32_t start = START + (STEPS / process_count) * rank + 
		((STEPS % process_count) < rank ? (STEPS % process_count) : rank) + 1;
	uint32_t end = start + (STEPS / process_count) + 
		((STEPS % process_count) > rank) - 1;
	uint32_t hits = 0;

	/* Iterate over range and sum up single slices*/
	for (uint32_t i=start; i<=end; i++) {
		hits += is_valid_id(i);
	}

	printf("%d acceptable IDs found in range [%d, %d].\n", hits, start, end);

	/* Code for master process */
	if (rank == 0) {
		uint32_t remote_hits;

		/* Wait for process_count-1 processes to finish and add their results to
		   local sum */
		for (int source = 1; source < process_count; source++) {
			MPI_Recv(&remote_hits, 1, MPI_DOUBLE, source, 0, 
				MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			hits += remote_hits;
		}

		printf("%d acceptable IDs found in total.\n", hits);

	} 

	/* Code for worker process */
	else {
		/* Send result to master (process 0) */
		MPI_Send(&hits, 1, MPI_INTEGER4, 0, 0, MPI_COMM_WORLD);
	}

	/* MPI finalize */
	MPI_Finalize();

	return 0;

}