
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "data_types.h"

void Isend_dataset(DataSet *dataSet, int dest, /*out*/ MPI_Request **requests);
void Ireceive_dataset(DataSet *dataSet, int src, int D, MPI_Request **request);
void wait_for_request(MPI_Request **request, int count);

void send_dataset(DataSet *dataSet, int dest);
void receive_dataset(DataSet *dataSet, int src, int D);


int MPI_read_data( char* filename, DataSet* dataSet, int rank, int size)
{
	int D=0, N=0;
	MPI_File fh;
	MPI_Status status;
	//"./data/bin_data/mnist_train_svd.bin"
	MPI_File_open( MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
	if(fh <= 0)
		printf("[MPI_KNN]: ERROR OPENING FILE\n");
		
	// Read Header
	MPI_File_set_view(fh, 0, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
	MPI_File_read_at(fh, 0, &N, 1, MPI_INT, &status);
	MPI_File_read_at(fh, 1, &D, 1, MPI_INT, &status);
	

	// Allocate sub-dataset 
	int subN = N/size;
	int mod = N % size;
	if(rank==(size-1))
		subN = subN + mod; //The last process will take the modulo of the N/size div

	allocate_empty_dataset(dataSet, subN,D);

	// Read Main body and store it in the dataset. Refere to data_types.h for more info.
	MPI_File_set_view(fh, 0, MPI_DOUBLE, MPI_DOUBLE, "native", MPI_INFO_NULL); 
	
	if(rank == size-1)
		MPI_File_read_at(fh, 1+rank*(subN-mod)*D, dataSet->data, subN*D, MPI_DOUBLE, &status);
	else
		MPI_File_read_at(fh, 1+rank*(subN*D), dataSet->data, subN*D, MPI_DOUBLE, &status);

	MPI_File_close(&fh);

	// Calc the datapoint index of the dataset.
	int i;
	if(rank == size-1)
	{
		for(i=0;i<dataSet->N; i++)
			dataSet->index[i] = i + rank*(subN-mod) +1;
	}
	else
	{
		for(i=0;i<dataSet->N; i++)
			dataSet->index[i] = i + rank*(subN) +1;		
	}
	
	return mod;
}


/**
	Non-Blocking Send operation of a dataSet struct. 
	
	The function retursn an array[2] of MPI_requests instances. 
	It is important to handle these structs with care and free them on the appropriate moment
	
	DataSet dataSet = the original Dataset struct.
	int dest = mpi destination process 
	int start = 0-based index of the first dataPoint of the dataset to be sent.
	int subN = The total length of the sub-dataset to be sent.
	MPI_Request requets** = array of mpi requests to handle the "data received" event.

	ref: http://mpitutorial.com/tutorials/dynamic-receiving-with-mpi-probe-and-mpi-status/
*/
void Isend_dataset(DataSet *dataSet, int dest, /*out*/ MPI_Request **request)
{	
	int D = dataSet->D;
	int N = dataSet->N;
	*request = (MPI_Request*) malloc(2*sizeof(MPI_Request));

	MPI_Isend( dataSet->data, D*N, MPI_DOUBLE, dest, DATA_TAG,  \
		MPI_COMM_WORLD,  &(*request)[0]);
	MPI_Isend( dataSet->index,  N,   MPI_INT,    dest, INDEX_TAG, \
		MPI_COMM_WORLD,  &(*request)[1]);
}


/**
	Non-Blocking Receive operation for a dataSet. 

	This function handles the memory allocation. 
	
	The function retursn an array[3] of MPI_requests instances. 
	It is important to handle these structs with care and free them on the appropriate moment
	
	DataSet dataSet = the original Dataset struct.
	int src = mpi source process 
	MPI_Request requets** = array of mpi requests to handle the "data received" event.

	ref: http://mpitutorial.com/tutorials/dynamic-receiving-with-mpi-probe-and-mpi-status/
*/
void Ireceive_dataset(DataSet *dataSet, int src, int D, /*out*/ MPI_Request **request){
	MPI_Status status;
	int N;
	
	/*Probe for dataset length*/
	MPI_Probe(src, INDEX_TAG, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_INT, &N);

	/*Allocate Mem Appropriately*/
	allocate_empty_dataset(dataSet,N,D);
	
	/*Receive like cray-cray */
	*request = (MPI_Request*) malloc(2*sizeof(MPI_Request));
	MPI_Irecv(dataSet->data, N*D, MPI_DOUBLE, src, DATA_TAG,  MPI_COMM_WORLD, &(*request)[0]);
	MPI_Irecv(dataSet->index, N, MPI_INT, 	  src, INDEX_TAG, MPI_COMM_WORLD, &(*request)[1]);
}

void wait_for_request(MPI_Request **request, int count)
{
	int r;
	MPI_Status status;
	for(r=0; r<count; r++)
		MPI_Wait(&(*request)[r], &status);
	//printf("Complete Dataset has been received\n");
}


/**
	Blocking Send operation for a subset of the whole dataSet. 
	
	DataSet dataSet = the original Dataset struct.
	int dest = mpi destination process 
*/
void send_dataset(DataSet *dataSet, int dest)
{
	MPI_Status status;
	int D = dataSet->D;
	int N = dataSet->N;
	
	MPI_Send( dataSet->data, D*N, MPI_DOUBLE, dest, DATA_TAG, MPI_COMM_WORLD);
	MPI_Send( dataSet->index,  N,   MPI_INT,    dest, INDEX_TAG, MPI_COMM_WORLD);
}

/**
	Non-Blocking Receive operation for a dataSet. 

	This function handles the memory allocation. 
	
	DataSet dataSet = the original Dataset struct.
	int src = mpi source process 
*/
void receive_dataset(DataSet *dataSet,  int src, int D){
	MPI_Status status;
	int N;
	
	// WARNING: Need to implement this differently
	MPI_Probe(src, INDEX_TAG, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_INT, &N);

	/*Allocate Appropriately*/
	allocate_empty_dataset(dataSet,N, D);

	/*Receive like cray-cray */
	MPI_Recv(dataSet->index, N, MPI_INT, 	 src, INDEX_TAG, MPI_COMM_WORLD, &status);
	MPI_Recv(dataSet->data, N*D, MPI_DOUBLE, src, DATA_TAG,  MPI_COMM_WORLD, &status);
}
