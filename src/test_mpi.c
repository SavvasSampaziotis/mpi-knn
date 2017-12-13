
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


	/* Set up some configuration utilities*/
	int destRank = rank + 1;
	int srcRank = rank - 1;
	if (rank == size-1)	
		destRank = 0;
	if (rank == 0)
		srcRank = size-1;

	
	printf("---------TYPE CREATED\n");
	

	if(rank==0)
	{	

		DataSet dataSet;
		readDataDUMMY( &dataSet, N,D);
		
		MPI_Send( dataSet.data, N*D, MPI_DOUBLE, destRank, 0, MPI_COMM_WORLD);

		printDataSet( &dataSet );
		/*
		int i;
		for (i = 0; i<N*D; i++)
			printf("%lf\n", dataSet.data[i]);
		*/
		printf("---------SENT\n");
	}
	else if(rank==1)
	{
		MPI_Status status;	

		DataSet recvDataset;
		allocateEmptyDataSet( &recvDataset, N,D); // Init dataset array
		
		MPI_Recv( recvDataset.data, N*D, MPI_DOUBLE, srcRank, 0, MPI_COMM_WORLD, &status);

		int i;
		for (i = 0; i<N*D; i++)
			printf("%lf\n", recvDataset.data[i]);

		printDataSet( &recvDataset);
		

		printf("---------RECEIVED\n");

	}
	

	//MPI_Type_free(&MPI_DATAPOINT_INDEX_LABEL);
	//MPI_Type_free(&MPI_DATAPOINT_POINT);

	//printf("SAVVAS\n");

	MPI_Finalize();
	return 	0;
}

