
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <mpi/mpi.h>

#include "data_types.h"


struct timeval startwtime, endwtime;

int main(int argc, char** argv){

	/* Init MPI */
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	

	if(rank==0)
	{
		/* Read all Data and Distribute among the rest of the processes*/
		DataSet dataSet;
		readData("./data/formatted_data/mnist_train.txt", &dataSet);
		distributeData(&dataSet, receivedDataSet);	
	}
		
	printf("---------TYPE CREATED\n");
	
	

	MPI_Type_commit(&MPI_DATAPOINT);
	
	if(rank==0)
	{	

		MPI_Send( dataSet.data, N*D, MPI_DOUBLE, destRank, 0, MPI_COMM_WORLD);

		MPI_Send( dataSet.dataPoints, N, MPI_DATAPOINT, destRank, 0, MPI_COMM_WORLD);

		printDataSet( &dataSet );
		printf("---------SENT\n");

	}
	else if(rank==1)
	{
		MPI_Status status;	

		DataSet recvDataset;
		allocateEmptyDataSet( &recvDataset, N,D); // Init dataset array
		
		MPI_Recv( recvDataset.data, N*D, MPI_DOUBLE, srcRank, 0, MPI_COMM_WORLD, &status);

		MPI_Recv( recvDataset.dataPoints, N, MPI_DATAPOINT, srcRank, 0, MPI_COMM_WORLD, &status);

		printDataSet( &recvDataset);
		

		printf("---------RECEIVED\n");

	}
	

	
	MPI_Type_free(&MPI_DATAPOINT);

	MPI_Finalize();
	return 	0;
}


void distributeData(DataSet* dataSet, DataSet* receivedDataSet)
{
	for(i=0;i<)
}


void createMPI_DATAPOINT(MPI_Datatype *MPI_DATAPOINT){
	
	MPI_Datatype tempType;
	DataSet	tempDataSet;

	// THe dimensionality of the dataset does not affect the struct-dataPoint length. 
	allocateEmptyDataSet(&tempDataSet, 2, 1); 

	// 
	MPI_Type_contiguous(2, MPI_INT, &tempType);
	
	MPI_Aint ub = (MPI_Aint) &(dataSet.dataPoints[1]) - (MPI_Aint) &(dataSet.dataPoints[0]);
	
	MPI_Type_create_resized(tempType, 0, ub, MPI_DATAPOINT);

	
	deAllocateDataSet(&tempDataSet);
}