
#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>

#include "data_types.h"


void send_subdataset(DataSet *dataSet, int dest, int start, int subN);
void receive_dataset(DataSet *dataSet, int src, int D);

void Isend_subdataset(DataSet *dataSet, int dest, int start, int subN, MPI_Request **request);
void Ireceive_dataset(DataSet *dataSet, int src, int D, MPI_Request **request);




/**
	Non-Blocking Send operation for a subset of the complete dataSet. 
	
	The function retursn an array[3] of MPI_requests instances. 
	It is important to handle these structs with care and free them on the appropriate moment
	
	DataSet dataSet = the original Dataset struct.
	int dest = mpi destination process 
	int start = 0-based index of the first dataPoint of the dataset to be sent.
	int subN = The total length of the sub-dataset to be sent.
	MPI_Request requets** = array of mpi requests to handle the "data received" event.

	ref: http://mpitutorial.com/tutorials/dynamic-receiving-with-mpi-probe-and-mpi-status/
*/
void Isend_subdataset(DataSet *dataSet, int dest, int start, int subN, /*out*/ MPI_Request **request)
{	
	int D = dataSet->D;
	//Send size first, so that we can preallocate accordingly.
	//MPI_Send( &subN, 1, MPI_INT, dest, 0, MPI_COMM_WORLD );
	//MPI_Send( &D, 	 1, MPI_INT, dest, 1, MPI_COMM_WORLD );
	
	*request = (MPI_Request*) malloc(3*sizeof(MPI_Request));

	MPI_Isend( &(dataSet->data[start*D]), D*subN, MPI_DOUBLE, dest, DATA_TAG,  \
		MPI_COMM_WORLD,  &(*request)[0]);
	MPI_Isend( &(dataSet->label[start]),  subN,   MPI_INT,    dest, LABEL_TAG, \
		MPI_COMM_WORLD,  &(*request)[1]);
	MPI_Isend( &(dataSet->index[start]),  subN,   MPI_INT,    dest, INDEX_TAG, \
		MPI_COMM_WORLD,  &(*request)[2]);
}

/**
	Send non-blockingly a whole dataset to mpi process with rank="dest"
	
	The function retursn an array[3] of MPI_requests instances. 
	It is important to handle these structs with care and free them on the appropriate moment

	Wrapper function of Isend_subdataset.
*/
void Isend_dataset(DataSet *dataSet, int dest, /*out*/ MPI_Request **requests)
{
	Isend_subdataset(dataSet, dest, 0, dataSet->N, requests);
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
	
	MPI_Probe(src, LABEL_TAG, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_INT, &N);

	/*Allocate Mem Appropriately*/
	allocate_empty_dataset(dataSet,N,D);
	
	/*Receive like cray-cray */
	*request = (MPI_Request*) malloc(3*sizeof(MPI_Request));
	MPI_Irecv(dataSet->label, N, MPI_INT, 	  src, LABEL_TAG, MPI_COMM_WORLD, &(*request)[1]);
	MPI_Irecv(dataSet->index, N, MPI_INT, 	  src, INDEX_TAG, MPI_COMM_WORLD, &(*request)[2]);
	MPI_Irecv(dataSet->data, N*D, MPI_DOUBLE, src, DATA_TAG,  MPI_COMM_WORLD, &(*request)[0]);
}

void wait_for_request(MPI_Request **request, int count)
{
	int r;
	for(r=0; r<count; r++)
		MPI_Wait(&(*request)[r], NULL);
	//printf("Complete Dataset has been received\n");
}





/**
	Blocking Send operation for a subset of the whole dataSet. 
	
	DataSet dataSet = the original Dataset struct.
	int dest = mpi destination process 
	int start = 0-based index of the first dataPoint of the dataset to be sent.
	int subN = The total length of the sub-dataset to be sent.
*/
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
	Wrapper function of send_subdataset.
*/
void send_dataset(DataSet *dataSet, int dest)
{
	send_subdataset(dataSet, dest, 0, dataSet->N);
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
	
	MPI_Probe(src, LABEL_TAG, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_INT, &N);

	/*Allocate Appropriately*/
	allocate_empty_dataset(dataSet,N, D);

	/*Receive like cray-cray */
	MPI_Recv(dataSet->label, N, MPI_INT, 	 src, LABEL_TAG, MPI_COMM_WORLD, &status);
	MPI_Recv(dataSet->index, N, MPI_INT, 	 src, INDEX_TAG, MPI_COMM_WORLD, &status);
	MPI_Recv(dataSet->data, N*D, MPI_DOUBLE, src, DATA_TAG,  MPI_COMM_WORLD, &status);
}




/**	
	This function distributes the entire dataset to all processes.

	The senging operation is BLOCKING.

	For the use of Process with RANK=0 ONLY.
*/
void distribute_data(DataSet* dataSet, int rank, int size, int D)
{	
	if(rank==0)
	{	
		
		int subN = dataSet->N/size;
		int mod = dataSet->N % size;
		int dest;
		
		for(dest=1; dest<(size-1); dest++)
		{
			int start = dest*subN;
			MPI_Request *request;
			send_subdataset(dataSet, dest, start, subN);
		}
		
		//Send last DataSet, of length [N/size + mod(N,size)]
		MPI_Request *request;
		
		send_subdataset(dataSet, size-1, (size-1)*subN, subN+mod);
	}
	else
	{	
		// No need for Irecv here. NOthing to do except receive everything...
		receive_dataset(dataSet, 0, D);
	}
	
	// Very important to WAIT for the distribution to finish, before start the processing of the data.
	MPI_Barrier(MPI_COMM_WORLD);
}

/**	
	This function distributes the entire dataset to all processes.

	The sending operation is NON-BLOCKING, but the use of barrier makes no difference really. 
	
	The MPI_requests objects are immediately freed. There is no need for the wait() call, 
	thanks to an MPI_Barrier in use. 

	For the use of Process with RANK=0 ONLY.
*/
void Idistribute_data(DataSet* dataSet, int rank, int size, int D)
{	

	if(rank==0)
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

		/* The MPI_request objects are freed, because the MPI_Barrier renders them redundunt. */
	}
	else
	{	
		// No need for Irecv here. NOthing to do except receive everything...
		receive_dataset(dataSet, 0, D);
	}

	// Very important to WAIT for the distribution to finish, before start the processing of the data.
	MPI_Barrier(MPI_COMM_WORLD);
}