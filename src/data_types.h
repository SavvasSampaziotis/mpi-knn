/**
 data_types.h

 Various Useful Data structs that will be used in .c files of the mpi-knn project  

	Author: Savvas Sampaziotis
*/
#ifndef DATA_TYPES_SAVVAS_H
#define DATA_TYPES_SAVVAS_H

typedef struct DataPointStruct{
	int index;
	int label;
	double *point; //Pointer pointing on a Dataset.data segment.    
} DataPoint;

typedef struct DataSetStruct
{	
	int N;
	int D;
	double* data; // Continous array all the data 
	DataPoint* dataPoints;
} DataSet;



typedef struct neighbourPointStruct{
	DataPoint* dpoint; //A dataPoint and its distance from another datapoint
	double dist;
} nnPoint;



#endif
 