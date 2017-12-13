
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
			(*dataSet).dataPoints[i].point[j] = temp;
		}

		int temp = -10;
		if (EOF == fscanf(fp, "%d\n", &temp))
				printf("ERROR Reading label in %d row",i);
		
		(*dataSet).dataPoints[i].label = temp;
		(*dataSet).dataPoints[i].index = i;
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
		(*dataSet).dataPoints[i].index = i;
		(*dataSet).dataPoints[i].label = i;
		for(j=0; j<D; j++)
			(*dataSet).dataPoints[i].point[j] = i+j;
	}
}


void allocateEmptyDataSet(DataSet* dataSet, int N, int D){
	int i,j;
	
	dataSet->data = (double*) malloc(N*D*sizeof(double));
	dataSet->N = N;
	dataSet->D = D;
	dataSet->dataPoints = (DataPoint*) malloc(N*sizeof(DataPoint));

	for(i=0; i<N; i++)
	{
		dataSet->dataPoints[i].index = -1;
		dataSet->dataPoints[i].label = -1;

		// Each dataPoint points to a row of the whole data matrix
		(*dataSet).dataPoints[i].point = &((*dataSet).data[D*i]);
		for(j=0; j<D; j++)
			dataSet->dataPoints[i].point[j] = 11112;
	}

}

void printDataPoint(DataPoint dp, int D)
{
	int i;
	printf("\t%d: [", dp.index);
	for (i = 0; i < D; ++i)
		printf("%lf ",dp.point[i]);	
	printf("]\t%d", dp.label);
	printf("\n");
}


/**
	Prints array of NxD elements 
*/
void printDataSet(DataSet* dataSet)
{
	int i;
	for (i = 0; i<dataSet->N; i++){
		printDataPoint(dataSet->dataPoints[i], dataSet->D);
		printf("\n");
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

