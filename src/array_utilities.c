
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_types.h"

/*
void allocateArray(double** A, int N, int D)
{
	int i;
	
	A = (double **) malloc(N*sizeof(double*));
	
	for (i = 0; i<N; i++){
		//double* temp = (double *) malloc(D*sizeof(double));
		(A[i]) = (double *) malloc(D*sizeof(double));
	}	
}
*/

/**
	Reads data from file. 

	Reference COde from Tutorials Point and  http://www.cplusplus.com/reference/

*/
void readData(const char* filename, dataPoint **dataSet, int* N, int* D )
{
	// Open File
	FILE *fp;
	fp = fopen( filename, "r" );

	if(fp == 0){
		printf("PROBLEM Opening file\n");
		return;
	}

	// Read File Header and parse dataSet size and dimensionality
	char buff[15];
	fscanf(fp, "%d\t%d\n", N,D);
	printf("N=%d \t D=%d \n", *N,*D);
	//*N = 10;	
	//*D = 3;

	*dataSet = (dataPoint*) malloc((*N)*sizeof(dataPoint));
	int i,j;	
	for(i=0; i<(*N); i++)
	{
		(*dataSet)[i].point = (double*) malloc((*D)*sizeof(double));
		
		for(j=0; j<(*D); j++)
		{
			double  temp = 3;;
			if (EOF == fscanf(fp, "%lf\t", &temp))
				printf("ERROR Reading datapoint in %d row, %d column",i,j);
			(*dataSet)[i].point[j] = temp;
		}

		int temp = -10;
		if (EOF == fscanf(fp, "%d\n", &temp))
				printf("ERROR Reading label in %d row",i);
		
		(*dataSet)[i].label = temp;
	}

	fclose( fp );
}

/**
	Reads data from file...
*/
void readDataDUMMY(dataPoint **dataSet, int N, int D){
	int i,j;
	
	*dataSet = (dataPoint*) malloc(N*sizeof(dataPoint));
	
	for(i=0; i<N; i++)
	{
		(*dataSet)[i].label = i;
		(*dataSet)[i].point = (double*) malloc(D*sizeof(double));
		for(j=0; j<D; j++)
			(*dataSet)[i].point[j] = i+j;
	}
}


void printDataPoint(dataPoint dp, int D)
{
	int i;
	for (i = 0; i < D; ++i)
		printf("%lf ",dp.point[i]);	
	printf("\t%d", dp.label);
	printf("\n");
}


/**
	Prints array of NxD elements 
*/
void printDataset(dataPoint* dataSet, int N, int D)
{
	int i;
	for (i = 0; i<N; i++){
		printDataPoint(dataSet[i],D);
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

