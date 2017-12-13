
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <mpi/mpi.h>

#include "data_types.h"

void distribute_data(DataSet* dataSet, int size);
void send_dataset(DataSet *dataSet,int dest, int start, int subN, int D);
void receive_dataset(DataSet *dataSet);


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
		//read_data("./data/formatted_data/mnist_train.txt", &dataSet);
		read_data_DUMMY(&dataSet, 10, 3);
		print_dataset(&dataSet);	
		//printf("RANK: %d\tSending Dataset...\n", rank);
		distribute_data(&dataSet, size);
		//

	}else{
		DataSet recvDataSet;
		
		receive_dataset(&recvDataSet);

		printf("RANK: %d\tRecieving Dataset...\n", rank);
		print_dataset(&recvDataSet);
	}
		
	MPI_Finalize();
	return 	0;
}


void distribute_data(DataSet* dataSet, int size)
{
	int subN = dataSet->N/size;
	int mod = dataSet->N % size;
	int D = dataSet->D;

	int dest;
	for(dest=1; dest<(size-1); dest++)
	{
		int start = dest*subN;
		send_dataset(dataSet, dest, start, subN, D);
	}

	//Send last DataSet, of length [N/size + mod(N,size)]
	send_dataset(dataSet, size-1, (size-1)*subN, subN+mod, D);
}


void send_dataset(DataSet *dataSet, int dest, int start, int subN, int D)
{
	//Send size first, so that we can preallocate accordingly.
	MPI_Send( &subN, 1, MPI_INT, dest, 0, MPI_COMM_WORLD );
	MPI_Send( &D, 		1, MPI_INT, dest, 1, MPI_COMM_WORLD );

	MPI_Request request[3];
	
	MPI_Isend( &(dataSet->data[start*D]), D*subN, MPI_DOUBLE, dest, DATA_TAG,  \
		MPI_COMM_WORLD, &request[0]);
	MPI_Isend( &(dataSet->label[start]),  subN,   MPI_INT,    dest, LABEL_TAG, \
		MPI_COMM_WORLD,  &request[1]);	
	MPI_Isend( &(dataSet->index[start]),  subN,   MPI_INT,    dest, INDEX_TAG, \
		MPI_COMM_WORLD,  &request[2]);
}


/**
	ref: http://mpitutorial.com/tutorials/dynamic-receiving-with-mpi-probe-and-mpi-status/
*/
void receive_dataset(DataSet *dataSet){
	MPI_Status status;
	MPI_Request request[3];
	int N, D, ND=0;
	
	//MPI_Bcast(&D, 1, MPI_INT, 0, MPI_COMM_WORLD );
	MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	MPI_Recv(&D, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
	ND = N*D;

	printf("Input DataSet : %d x %d = %d \n",N,D,ND);

	/*Allocate Appropriately*/
	allocate_empty_dataset(dataSet,N,D);

	/*Receive like cray-cray */
	MPI_Irecv(dataSet->data, 15, MPI_DOUBLE, 0, DATA_TAG,  MPI_COMM_WORLD, &request[0]);
	MPI_Irecv(dataSet->label, N, MPI_INT, 	0, LABEL_TAG, MPI_COMM_WORLD, &request[1]);
	MPI_Irecv(dataSet->index, N, MPI_INT, 	0, INDEX_TAG, MPI_COMM_WORLD, &request[2]);
	
	int r;
	for(r=0; r<3; r++)
	{
		//printf("Message %d has been received\n",r);
		MPI_Wait(&request[r], NULL);
	}
	//printf("Complete Dataset has been received\n");
}