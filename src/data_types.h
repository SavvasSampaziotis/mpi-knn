/**
 data_types.h

 Various Useful Data structs that will be used in .c files of the mpi-knn project  

	Author: Savvas Sampaziotis
*/
#ifndef DATA_TYPES_SAVVAS_H
#define DATA_TYPES_SAVVAS_H

typedef struct dataPointStruct{
	int index;
	int label;
	double *point;
} dataPoint;

typedef struct neighbourPointStruct{
	dataPoint* dpoint;
	double dist;
} nnPoint;



#endif
 