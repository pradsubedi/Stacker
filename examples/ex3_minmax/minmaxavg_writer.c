/* minmax_writer.c : Example 3: DataSpaces put 128 array
 * Bounding Box: 0,0,0 - 127,0,0 
 * 1 element at each space in the box
 * */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "dataspaces.h"
#include "mpi.h"
// Size of array, if changing
// MUST also change in minmax_reader.c
#define ARRAY_SIZE 128

int main(int argc, char **argv)
{
	int err;
	int nprocs, rank;
	MPI_Comm gcomm;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Barrier(MPI_COMM_WORLD);
	gcomm = MPI_COMM_WORLD;

	// Initalize DataSpaces
	// # of Peers, Application ID, ptr MPI comm, additional parameters
	// # Peers: Number of connecting clients to the DS server
	// Application ID: Unique idenitifier (integer) for application
	// Pointer to the MPI Communicator, allows DS Layer to use MPI barrier func
	// Addt'l parameters: Placeholder for future arguments, currently NULL.
	dspaces_init(1, 1, &gcomm, NULL);

	// Timestep notation left in to demonstrate how this can be adjusted
	int timestep=0;

	while(timestep<1){
		timestep++;

		// DataSpaces: Lock Mechanism
		// Usage: Prevent other process from modifying 
		// 	  data at the same time as ours
		dspaces_lock_on_write("my_test_lock", &gcomm);

		//Name the Data that will be writen
		char var_name[128];
		sprintf(var_name, "ex3_sample_data");

		// Initialize Random Number Generator
		srand(time(NULL));
		
		// Create integer array, size 128
		int *data = malloc(ARRAY_SIZE*sizeof(int));

		//Populate array, 128 integer, values 0-64k
		int i;
		for(i=0;i<ARRAY_SIZE;i++){
			data[i] = rand()%65536;
		}

		// ndim: Dimensions for application data domain
		// In this case, our data array will be 1 dimensional
		int ndim = 1; 
		
		// Prepare LOWER and UPPER bound dimensions, init 0s
		uint64_t lb[3] = {0}, ub[3] = {0};

		// Bounds: 0,0,0 to 127,0,0
		ub[0] = ARRAY_SIZE-1;

		// DataSpaces: Put data array into the space
		// 1 integer in each box, fill boxes 0,0,0 to 127,0,0
		dspaces_put(var_name, timestep, sizeof(int), ndim, lb, ub, data);

		free(data);
		
		// DataSpaces: Release our lock on the data
		dspaces_unlock_on_write("my_test_lock", &gcomm);
	}

	// DataSpaces: Finalize and clean up DS process
	dspaces_finalize();

	MPI_Barrier(gcomm);
	MPI_Finalize();

	return 0;
}
