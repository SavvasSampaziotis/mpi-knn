
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

	dataPoint *dataSet;
	readDataDUMMY(&dataSet, N,D);
	//printDataset( dataSet,  N, D );

	/* Print struct pointers to show memory allocation*/
	/*
	printf("DataPoint Address: %d\n", &(dataSet[0]));
	printf("DataPoint Address: %d\n", &(dataSet[0].index));
	printf("DataPoint Address: %d\n", &(dataSet[2].label));
	printf("DataPoint Address: %d\n", &(dataSet[2].point));
	printf("DataPoint Address: %d\n", &(dataSet[2].point[0]));
	printf("DataPoint Address: %d\n\n\n", &(dataSet[2].point[1]));
	*/
	
	/* Set up some configuration utilities*/
	int destRank = rank + 1;
	int srcRank = rank - 1;
	if (rank == size-1)	
		destRank = 0;
	if (rank == 0)
		srcRank = size-1;

	MPI_Datatype MPI_DATAPOINT_INDEX_LABEL, MPI_DATAPOINT_POINT, istruct;
	
	/*
	const int nfields=2;
    MPI_Aint disps[nfields];
    int blocklens[] = {1,1};
    MPI_Datatype types[] = {MPI_INT, MPI_INT};
    disps[0] = (MPI_Aint) offsetof( dataPoint, index );
    disps[1] = (MPI_Aint) offsetof( dataPoint, label );
    MPI_Type_create_struct(nfields, blocklens, disps, types, &istruct);
	*/   

   	MPI_Type_contiguous(2, MPI_INT, &istruct);
   	MPI_Type_create_resized( istruct, 0, \
     (char *)&(dataSet[1]) - (char *)(&dataSet[0]), &MPI_DATAPOINT_INDEX_LABEL );
   	// MPI_Type_commit(&MPI_DATAPOINT_INDEX_LABEL);

    MPI_Type_contiguous(D, MPI_DOUBLE, &MPI_DATAPOINT_POINT);
	//MPI_Type_create_resized( istruct, offsetof(dataPoint, point), 
    // (char *)&(dataSet[1]) - (char *)(&dataSet[0]), &MPI_DATAPOINT_INDEX_LABEL );
    MPI_Type_commit(&MPI_DATAPOINT_POINT);
	////printf("---------TYPE CREATED\n");

	if(rank==0)
	{	
		//MPI_Send( dataSet, N, MPI_DATAPOINT_INDEX_LABEL, destRank, 0, MPI_COMM_WORLD);

		// Buff contains the pointers of all point-arrays of datapoint struct
		/*double** buff = (double**) malloc(D*sizeof(double*));
		int i;
		for(i=0;i<N;i++)
			buff[i] = dataSet[i].point;*/
		//MPI_Send( buff, N, MPI_DATAPOINT_POINT, destRank, 1, MPI_COMM_WORLD);
		
		//printArray(buff,N,D);
		printf("---------SENT\n");
	}
	else if(rank==1)
	{
		MPI_Status status;	

		dataPoint *recvDataset;
		allocateEmptyDataset(&recvDataset, N,D); // Init dataset array
		//MPI_Recv( recvDataset, N, MPI_DATAPOINT_INDEX_LABEL, srcRank, 0, MPI_COMM_WORLD, &status);

		double** buff = (double**) malloc(N*sizeof(double*));
	//	MPI_Recv( buff, N, MPI_DATAPOINT_POINT, srcRank, 1, MPI_COMM_WORLD, &status);
		int i;
		for(i=0;i<N;i++){
			//printf("RECEIVED BUFF: %lf\n", buff[i][2]);
			//recvDataset[i].point = buff[i];
		}		


		//printDataset(recvDataset,N,D);
		//printArray(buff,N,D);
		printf("---------RECEIVED\n");

	}
	

	//MPI_Type_free(&MPI_DATAPOINT_INDEX_LABEL);
	//MPI_Type_free(&MPI_DATAPOINT_POINT);

	//printf("SAVVAS\n");

	MPI_Finalize();
	return 	0;
}

