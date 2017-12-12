
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

	/* Set up some configuration utilities*/
	int destRank = rank + 1;
	int srcRank = rank - 1;
	if (rank == size-1)	
		destRank = 0;
	if (rank == 0)
		srcRank = size-1;

	MPI_Datatype MPI_DATAPOINT_A, MPI_DATAPOINT_B;

	const int nfields=2;
    MPI_Aint disps[nfields];
    int blocklens[] = {1,1};
    MPI_Datatype types[] = {MPI_INT, MPI_INT};
    disps[0] = (MPI_Aint) offsetof( dataPoint, index );
    disps[1] = (MPI_Aint) offsetof( dataPoint, label );
    MPI_Type_create_struct(nfields, blocklens, disps, types, &MPI_DATAPOINT_A);
    MPI_Type_commit(&MPI_DATAPOINT_A);

    
    /*MPI_Datatype typeB[] = {MPI_DOUBLE};
    MPI_Aint dispB = (MPI_Aint) offsetof( dataPoint, point );
    MPI_Type_create_struct( 1, &D, &dispB, typeB, &MPI_DATAPOINT_B);
    
	*/
	
	MPI_Type_contiguous(D, MPI_DOUBLE, &MPI_DATAPOINT_B);
	MPI_
	
	int test;
    MPI_Type_size(MPI_DATAPOINT_B, &test);
	printf("SIZE = %d\n", test );
	MPI_Type_commit(&MPI_DATAPOINT_B);

	printf("---------TYPE CREATED\n");

	//N = 1;
	if(rank==0)
	{	
		MPI_Send( dataSet, 1, MPI_DATAPOINT_A, destRank, 0, MPI_COMM_WORLD);

		double * test = (double*) malloc(D*sizeof(double));
    	test[0] = 11;
		MPI_Send( test, N, MPI_DATAPOINT_B, destRank, 1, MPI_COMM_WORLD);
		printf("---------SENT\n");
	}
	else if(rank==1)
	{
		dataPoint *recvDataset;
		//readDataDUMMY(&recvDataset, N,D);
		(*recvDataset).point = (double*) malloc(D*sizeof(double));
		//printDataPoint(recvDataset[3],D);

		MPI_Status status;
		MPI_Recv( recvDataset, N, MPI_DATAPOINT_A, srcRank, 0, MPI_COMM_WORLD, &status);

		double * test = (double*) malloc(D*sizeof(double));
		MPI_Recv( recvDataset, 1, MPI_DATAPOINT_B, srcRank, 1, MPI_COMM_WORLD, &status);
		
		printf("---------RECEIVED\n");
		printDataset(recvDataset, N, D);
		//printDataPoint(recvDataset[3],D);
		
		printf("label=%d\n", recvDataset[0].label);
		printf("index=%d\n", recvDataset[0].index);
		//printf("point=%lf\n", recvDataset[0].point[0]);
		//printf("point=%lf\n", test[0]);
	}
	

	MPI_Type_free(&MPI_DATAPOINT_A);
	MPI_Type_free(&MPI_DATAPOINT_B);


	MPI_Finalize();

	return 	0;
}
