/**
 data_types.h

 Various Nessesary Data structs that will be used in .c files of the mpi-knn project  

	Author: Savvas Sampaziotis
*/
#ifndef DATA_TYPES_SAVVAS_H
#define DATA_TYPES_SAVVAS_H


#define DATA_TAG 3
#define INDEX_TAG 5

/* 
As opsetai to Stack Overflow: 
https://stackoverflow.com/questions/5901476/sending-and-receiving-2d-array-over-mpi
*/
typedef struct DataSetStruct
{	
	int N;
	int D;
	double* data; // Continous array of all the data. Perfect for MPI
	double** dataPoints; //Array of pointers pointing on segments of data.
	//int* label;  This is obsolete. Only indeces are actually useful, 
	// so to minimize comm-time this will be ommited... You can refer to git tag [XXX] fro more info.
	int* index;
} DataSet;



typedef struct neighbourPointStruct
{
	//DataPoint* dpoint; //A dataPoint and its distance from another datapoint
	int index;
	double dist;
} nnPoint;



#endif
 