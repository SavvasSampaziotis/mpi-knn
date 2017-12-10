
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "data_types.h"

#include <mpi/mpi.h>


struct timeval startwtime, endwtime;


int main(int argc, char** argv){

	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	/** Generate Dummy Data*/
	int N=10, D=1;
	dataPoint *dataSet;
	
	readDataDUMMY(&dataSet, N,D);
	printDataset( dataSet,  N, D );


	int destRank = rank + 1;
	int srcRank = rank - 1;
	if (rank == size-1)	
		destRank = 0;
	if (rank == 0)
		srcRank = size-1;

	double secretNum = 100.0+rank;
	MPI_Send( &secretNum, 1, MPI_DOUBLE, destRank, 0, MPI_COMM_WORLD);

	double msg;
	MPI_Status status;
	MPI_Recv( &msg, 1, MPI_DOUBLE, srcRank, 0, MPI_COMM_WORLD, &status);
//	printf("Process: %d received %lf\n",rank, msg);

	MPI_Finalize();

	return 	0;
}

