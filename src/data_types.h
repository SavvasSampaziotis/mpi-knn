/**
 data_types.h

 Various Useful Data structs that will be used in .c files of the mpi-knn project  

	Author: Savvas Sampaziotis
*/
#ifndef DATA_TYPES_SAVVAS_H
#define DATA_TYPES_SAVVAS_H

typedef struct DataSetStruct
{	
	int N;
	int D;
	
	double* data; // Continous array all the data 
	
	double** dataPoints;

	int* label;
	int* index;

} DataSet;



typedef struct neighbourPointStruct
{
	//DataPoint* dpoint; //A dataPoint and its distance from another datapoint
	int index;
	double dist;
} nnPoint;



#endif
 