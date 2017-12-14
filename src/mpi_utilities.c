
#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>

#include "data_types.h"


void send_subdataset(DataSet *dataSet, int dest, int start, int subN);
void receive_dataset(DataSet *dataSet, int src);

void Isend_subdataset(DataSet *dataSet, int dest, int start, int subN, MPI_Request **request);
void Ireceive_dataset(DataSet *dataSet, int src, MPI_Request **request);

/**
	For the use of Process with RANK=0 ONLY.
*/
void distribute_data(DataSet* dataSet, int size)
{
	int subN = dataSet->N/size;
	int mod = dataSet->N % size;
	int dest;
	
	for(dest=1; dest<(size-1); dest++)
	{
		int start = dest*subN;
		MPI_Request *request;
		send_subdataset(dataSet, dest, start, subN);
		/*
		Isend_subdataset(dataSet, dest, start, subN, &request);
		MPI_Request_free(&request[0]);
		MPI_Request_free(&request[1]);
		MPI_Request_free(&request[2]);
		free(request);*/
	}
	
	//Send last DataSet, of length [N/size + mod(N,size)]
	MPI_Request *request;
	
	send_subdataset(dataSet, size-1, (size-1)*subN, subN+mod);
	
	/*
	Isend_subdataset(dataSet, size-1, (size-1)*subN, subN+mod, &request);
	MPI_Request_free(&request[0]);
	MPI_Request_free(&request[1]);
	MPI_Request_free(&request[2]);
	free(request);
	*/
}

void Idistribute_data(DataSet* dataSet, int size)
{
	int subN = dataSet->N/size;
	int mod = dataSet->N % size;
	int dest;
	
	for(dest=1; dest<(size-1); dest++)
	{
		int start = dest*subN;
		MPI_Request *request;
		
		Isend_subdataset(dataSet, dest, start, subN, &request);
		MPI_Request_free(&request[0]);
		MPI_Request_free(&request[1]);
		MPI_Request_free(&request[2]);
		free(request);
	}
	
	//Send last DataSet, of length [N/size + mod(N,size)]
	MPI_Request *request;
	Isend_subdataset(dataSet, size-1, (size-1)*subN, subN+mod, &request);
	MPI_Request_free(&request[0]);
	MPI_Request_free(&request[1]);
	MPI_Request_free(&request[2]);
	free(request);
}

void Isend_subdataset(DataSet *dataSet, int dest, int start, int subN, MPI_Request **request)
{	
	int D = dataSet->D;
	//Send size first, so that we can preallocate accordingly.
	MPI_Send( &subN, 1, MPI_INT, dest, 0, MPI_COMM_WORLD );
	MPI_Send( &D, 	 1, MPI_INT, dest, 1, MPI_COMM_WORLD );
	
	*request = (MPI_Request*) malloc(3*sizeof(MPI_Request));

	MPI_Isend( &(dataSet->data[start*D]), D*subN, MPI_DOUBLE, dest, DATA_TAG,  \
		MPI_COMM_WORLD,  &(*request)[0]);
	MPI_Isend( &(dataSet->label[start]),  subN,   MPI_INT,    dest, LABEL_TAG, \
		MPI_COMM_WORLD,  &(*request)[1]);
	MPI_Isend( &(dataSet->index[start]),  subN,   MPI_INT,    dest, INDEX_TAG, \
		MPI_COMM_WORLD,  &(*request)[2]);
}

/**
	Wrapper function of
*/
void Isend_dataset(DataSet *dataSet, int dest, MPI_Request **requests)
{
	Isend_subdataset(dataSet, dest, 0, dataSet->N, requests);
}
/**
	ref: http://mpitutorial.com/tutorials/dynamic-receiving-with-mpi-probe-and-mpi-status/
*/
void Ireceive_dataset(DataSet *dataSet, int src, MPI_Request **request){
	MPI_Status status;
	int N, D;
	
	MPI_Recv(&N, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
	MPI_Recv(&D, 1, MPI_INT, src, 1, MPI_COMM_WORLD, &status);

	/*Allocate Appropriately*/
	allocate_empty_dataset(dataSet,N,D);
	
	/*Receive like cray-cray */
	*request = (MPI_Request*) malloc(3*sizeof(MPI_Request));
	MPI_Irecv(dataSet->data, N*D, MPI_DOUBLE, src, DATA_TAG,  MPI_COMM_WORLD, &(*request)[0]);
	MPI_Irecv(dataSet->label, N, MPI_INT, 	  src, LABEL_TAG, MPI_COMM_WORLD, &(*request)[1]);
	MPI_Irecv(dataSet->index, N, MPI_INT, 	  src, INDEX_TAG, MPI_COMM_WORLD, &(*request)[2]);
}

void send_subdataset(DataSet *dataSet, int dest, int start, int subN)
{	
	MPI_Status status;
	int D = dataSet->D;
	//Send size first, so that we can preallocate accordingly.
	MPI_Send( &subN, 1, MPI_INT, dest, 0, MPI_COMM_WORLD );
	MPI_Send( &D, 	 1, MPI_INT, dest, 1, MPI_COMM_WORLD );
	
	MPI_Send( &(dataSet->data[start*D]), D*subN, MPI_DOUBLE, dest, DATA_TAG,  \
		MPI_COMM_WORLD);
	MPI_Send( &(dataSet->label[start]),  subN,   MPI_INT,    dest, LABEL_TAG, \
		MPI_COMM_WORLD);
	MPI_Send( &(dataSet->index[start]),  subN,   MPI_INT,    dest, INDEX_TAG, \
		MPI_COMM_WORLD);
}

/**
	Wrapper function of
*/
void send_dataset(DataSet *dataSet, int dest)
{
	send_subdataset(dataSet, dest, 0, dataSet->N);
}
/**
	ref: http://mpitutorial.com/tutorials/dynamic-receiving-with-mpi-probe-and-mpi-status/
*/
void receive_dataset(DataSet *dataSet, int src){
	MPI_Status status;
	int N, D;
	
	MPI_Recv(&N, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
	MPI_Recv(&D, 1, MPI_INT, src, 1, MPI_COMM_WORLD, &status);

	/*Allocate Appropriately*/
	allocate_empty_dataset(dataSet,N,D);

	/*Receive like cray-cray */
	MPI_Recv(dataSet->data, N*D, MPI_DOUBLE, src, DATA_TAG,  MPI_COMM_WORLD, &status);
	MPI_Recv(dataSet->label, N, MPI_INT, 	 src, LABEL_TAG, MPI_COMM_WORLD, &status);
	MPI_Recv(dataSet->index, N, MPI_INT, 	 src, INDEX_TAG, MPI_COMM_WORLD, &status);
}

void wait_for_request(MPI_Request **request, int count)
{
	int r;
	for(r=0; r<count; r++)
		MPI_Wait(&(*request)[r], NULL);
	//printf("Complete Dataset has been received\n");
}