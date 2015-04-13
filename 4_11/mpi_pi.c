/*
 * mpi_pi.c
 * Author: Christian Würthner
 * Description: Homework Cahpter 4 Excercise 11
 */

#define STEPS           1000000
#define RANGE_START     0.
#define RANGE_END       1.
#define INTEGRAL(x, w)  w * (4 / (1 + pow(x * w + 0.5 * w, 2)))

#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>

double get_current_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL); 
	return tv.tv_sec * 1000. + tv.tv_usec / 1000.;

}

int main(int argc, char **argv) {
	int process_count, rank;

	/* MPI init */
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* Time measurement for master process */
	double start_time = 0.;
	if(rank == 0) {
		start_time = get_current_time();
	}

	/* Calculate boundaries and step width */
	double step_width = (RANGE_END - RANGE_START) / STEPS;
	double start = (STEPS / process_count) * rank + 
		((STEPS % process_count) < rank ? (STEPS % process_count) : rank);
	double end = start + (STEPS / process_count) + 
		((STEPS % process_count) > rank);
	double sum = 0.0;

	/* Iterate over range and sum up single slices*/
	for (long i=start; i<=end; i++) {
		sum += INTEGRAL(i, step_width);
	}

	/* Report status */
	printf("Calculated range [%f, %f], partitial sum: %f\n", 
		start*step_width, end*step_width, sum);

	/* Code for worker process */
	if (rank == 0) {
		double partSum;
		int source;

		/* Wait for process_count-1 processes to finish and add their results to
		   local sum */
		for (source = 1; source < process_count; source++) {
			MPI_Recv(&partSum, 1, MPI_DOUBLE, source, 0, 
				MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			sum += partSum;
		}

		double end_time = get_current_time();
		printf("All parts received. Result: %f\n", sum);
		printf("Time needed using %d processes: %fms\n", 
			process_count, end_time-start_time);
	} 

	/* Code for worker process */
	else {
		/* Send result to master (process 0) */
		MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}

	/* MPI finalize */
	MPI_Finalize();

	return 0;
}






