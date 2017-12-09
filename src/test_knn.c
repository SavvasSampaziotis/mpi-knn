
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "data_types.h"

void test_distance_matrix();




struct timeval startwtime, endwtime;

int N,D;	
	dataPoint *dataSet;

int main(int argc, char** argv){
	int i;

	N = 10;
	D = 3;
	
	readData(&dataSet, N,D);

	//for (i = 0; i<N; i++)
//		printDataPoint(dataSet[i],D);
	
	int K = 3;
	nnPoint** KNN;
	knn(&dataSet,  N,D,K, &KNN);

	for (i = 0; i<N; i++){
		printf("----\n");
		printDataPoint(*(KNN[i][0].dpoint), D);
		printDataPoint(*(KNN[i][1].dpoint), D);
		printDataPoint(*(KNN[i][2].dpoint), D);
		//printf("\tDist = %f\n", KNN[i][1].dpoint->point[0]);
		//printf("\tDist = %f\n", KNN[i][1].dist);
	}
		
	
	//test_distance_matrix();

	return 0;
}


/**
	Some Simple Benchmarking for the parallel calculation of the Distance Matric  
*/
void test_distance_matrix(){

	double  seq_time;

	nnPoint ** distMatrix;

	gettimeofday (&startwtime, NULL);

	distance_matrix(&dataSet, N,D, &distMatrix);

	gettimeofday (&endwtime, NULL);
 	seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);
	printf("\n Serial Time: %f\n", seq_time);

	int i;
	for(i=0; i<N; i++)
		free(distMatrix[i]);
	free(distMatrix);

	gettimeofday (&startwtime, NULL);
		distance_matrix_OMP(&dataSet, N,D, &distMatrix);
	gettimeofday (&endwtime, NULL);
 	seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);
	printf("\n Parallel Time: %f\n", seq_time);
	

	//printf("\n*** Distance Matrix ***\n");
	//printArray(distMatrix,N,N);
	

	for(i=0; i<N; i++)
		free(distMatrix[i]);
	free(distMatrix);

	

}

