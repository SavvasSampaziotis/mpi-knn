
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
	int N=10, D=4;
	DataSet dataSet;
	readDataDUMMY( &dataSet, N,D);

	/* Set up some configuration utilities*/
	int destRank = rank + 1;
	int srcRank = rank - 1;
	if (rank == size-1)	
		destRank = 0;
	if (rank == 0)
		srcRank = size-1;

	
	printf("---------TYPE CREATED\n");
	
	MPI_Datatype MPI_DATAPOINT, tempType;

	MPI_Type_contiguous(2, MPI_INT, &tempType);
	MPI_Aint ub = (MPI_Aint) &(dataSet.dataPoints[1]) - (MPI_Aint) &(dataSet.dataPoints[0]);
	MPI_Type_create_resized(tempType, 0, ub, &MPI_DATAPOINT);

	MPI_Type_commit(&MPI_DATAPOINT);
	
	/** Send just a Subset of the dataSet */
	N = N - 2;

	if(rank==0)
	{	
		/* 
		Send datapoints 1 ... N-1. Datapoints 0 and N are ommitted. 
		This is done for testing purposes 
		*/
		MPI_Send( &(dataSet.data[D]), (N-2)*D, MPI_DOUBLE, destRank, 0, MPI_COMM_WORLD);
		MPI_Send( &(dataSet.label[1]), N-2, MPI_INT, destRank, 0, MPI_COMM_WORLD);
		MPI_Send( &(dataSet.index[1]), N-2, MPI_INT, destRank, 0, MPI_COMM_WORLD);

		printDataSet( &dataSet );
		printf("---------SENT\n");

	}
	else if(rank==1)
	{
		MPI_Status status;	

		DataSet recvDataset;
		allocateEmptyDataSet( &recvDataset, N-2,D); // Init dataset array
		
		MPI_Recv( recvDataset.data, (N-2)*D, MPI_DOUBLE, srcRank, 0, MPI_COMM_WORLD, &status);

		MPI_Recv( recvDataset.label, N-2, MPI_INT, srcRank, 0, MPI_COMM_WORLD, &status);
		MPI_Recv( recvDataset.index, N-2, MPI_INT, srcRank, 0, MPI_COMM_WORLD, &status);

		printDataSet( &recvDataset);
		

		printf("---------RECEIVED\n");

	}
	

	
	MPI_Type_free(&MPI_DATAPOINT);

	//printf("SAVVAS\n");

	MPI_Finalize();
	return 	0;
}

