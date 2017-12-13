
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_types.h"

void allocateEmptyDataSet(DataSet* dataSet, int N, int D);


/**
	Reads data from file. 

	Reference Code from Tutorials Point and  http://www.cplusplus.com/reference/
	 regarding the use of fscan  

*/
void readData(const char* filename, DataSet *dataSet)
{
	// Open File
	FILE *fp;
	fp = fopen( filename, "r" );

	if(fp == 0){
		printf("PROBLEM Opening file\n");
		return;
	}

	// Read File Header and parse dataSet size and dimensionality
	int N,D;
	char buff[15];
	fscanf(fp, "%d\t%d\n", &N,&D);
	printf("N=%d \t D=%d \n", N,D);
	//*N = 10;	
	//*D = 3;
	allocateEmptyDataSet(dataSet,N,D);
	
	int i,j;	
	for(i=0; i<N; i++)
	{
		for(j=0; j<D; j++)
		{
			double  temp = 3;;
			if (EOF == fscanf(fp, "%lf\t", &temp))
				printf("ERROR Reading datapoint in %d row, %d column",i,j);
			dataSet->dataPoints[i][j] = temp;
		}

		int temp = -10;
		if (EOF == fscanf(fp, "%d\n", &temp))
				printf("ERROR Reading label in %d row",i);
		
		dataSet->label[i] = temp;
		dataSet->index[i] = i;
	}

	fclose( fp );
}

/**
	Reads data from file...
*/
void readDataDUMMY(DataSet *dataSet, int N, int D){
	int i,j;
	
	allocateEmptyDataSet(dataSet,N,D);
	
	for(i=0; i<N; i++)
	{
		dataSet->index[i] = i;
		dataSet->label[i] = i;
		for(j=0; j<D; j++)
			dataSet->dataPoints[i][j] = i+j;
	}
}


void allocateEmptyDataSet(DataSet* dataSet, int N, int D){
	int i,j;
	
	dataSet->data = (double*) malloc(N*D*sizeof(double));
	dataSet->N = N;
	dataSet->D = D;
	dataSet->dataPoints = (double**) malloc(N*sizeof(double*));

	dataSet->index = (int*) malloc(N*sizeof(int));
	dataSet->label = (int*) malloc(N*sizeof(int));

	for(i=0; i<N; i++)
	{
		dataSet->index[i] = -1;
		dataSet->label[i] = -1;

		// Each dataPoint points to a row of the whole data matrix
		dataSet->dataPoints[i] = &(dataSet->data[D*i]);
		for(j=0; j<D; j++)
			dataSet->dataPoints[i][j] = 11112;
	}

}


void deAllocateDataSet(DataSet* dataSet)
{	
	int i;

	// All Datapoint structs is now freed
	free(dataSet->dataPoints);
	
	// All data are now freed. Data has size [1,N*D], so one 'free()' will suffice.
	free(dataSet->data);
}
/*
void printDataPoint(DataPoint dp, int D)
{
	int i;
	printf("\t%d: [", dp.index);
	for (i = 0; i < D; ++i)
		printf("%lf ",dp.point[i]);	
	printf("]\t%d", dp.label);
	printf("\n");
}
*/

/**
	Prints array of NxD elements 
*/
void printDataSet(DataSet* dataSet)
{
	int i,j;
	for (i = 0; i<dataSet->N; i++)
	{
		printf("\t%d: [", dataSet->index[i]);
		for (j = 0; j < dataSet->D; j++)
		{
			printf("%lf ",dataSet->dataPoints[i][j]);	
		}
		printf("]\t%d\n", dataSet->label[i]);
	}
}



void printArray(double** A, int N, int M)
{
	int i,j;
	for (i = 0; i<N; i++){
		for (j = 0; j<M; j++)
			printf("%f ",(A)[i][j]);
		printf("\n");
	}
}
