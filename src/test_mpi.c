
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "data_types.h"

#include <mpi/mpi.h>


struct timeval startwtime, endwtime;


int main(int argc, char** argv){

	/* Init MPI */
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	/* Generate Dummy Data*/
	int N=10, D=2;

	dataPoint *dataSet;
	readDataDUMMY(&dataSet, N,D);
	//printDataset( dataSet,  N, D );

	/* Set up some configuration utilities*/
	int destRank = rank + 1;
	int srcRank = rank - 1;
	if (rank == size-1)	
		destRank = 0;
	if (rank == 0)
		srcRank = size-1;

	/*  Create DataType */
	
	// Data types contained in MPI struct
	MPI_Datatype array_of_types[] = {MPI_INT, MPI_INT, MPI_DOUBLE};
	

	int block_lengths[3]; // Count of each data type 
	block_lengths[0]= 1;
	block_lengths[1]= 1;
	block_lengths[2]= D; 
	
	MPI_Aint offsets[3]; // Offsets in addresses.
	dataPoint tempDP = dataSet[4];
	
	offsets[0] = offsetof( dataPoint, index );
	offsets[1] = offsetof( dataPoint, label );
	offsets[2] = offsetof( dataPoint, point );

	//printf("offset = %d %d %d\n", offsets[0], offsets[1], offsets[2]);
	printf("off %d\n", offsets[2]);
	// Create New Data type
	MPI_Datatype MPI_DATAPOINT;
	MPI_Type_struct( 3, block_lengths, offsets, array_of_types, &MPI_DATAPOINT);

	MPI_Type_commit(&MPI_DATAPOINT);
	
	printf("---------TYPE CREATED\n");

	N = 3;
	if(rank==0)
	{	
		MPI_Send( dataSet, N, MPI_DATAPOINT, destRank, 0, MPI_COMM_WORLD);
		printf("---------SENT\n");
	}
	else if(rank==1)
	{
		dataPoint *recvDataset;
		readDataDUMMY(&recvDataset, N,D);
		//recvDataset.point = (double*) malloc(D*sizeof(double));
		//printDataPoint(recvDataset[3],D);

		MPI_Status status;
		MPI_Recv( recvDataset, N, MPI_DATAPOINT, srcRank, 0, MPI_COMM_WORLD, &status);
		
		printf("---------RECEIVED\n");
		//printDataset(recvDataset,1, D);
		//printDataPoint(recvDataset[3],D);
		
		printf("label=%d\n", recvDataset[0].label);
		printf("index=%d\n", recvDataset[0].index);
		printf("point=%d\n", recvDataset[0].point[0]);
	}
	

	MPI_Type_free(&MPI_DATAPOINT);


	MPI_Finalize();

	return 	0;
}
