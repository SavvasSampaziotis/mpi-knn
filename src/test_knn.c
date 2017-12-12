
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "data_types.h"

void test_distance_matrix();




struct timeval startwtime, endwtime;

int N,D;	
DataSet dataSet;

int main(int argc, char** argv){
	int i;

	N = 10;
	D = 3;
	
	//readData("./data/formatted_data/mnist_train_svd.txt", dataSet, &N, &D);
	//readData("./data/formatted_data/mnist_train.txt", &dataSet);
	//readData("./data/formatted_data/testData.txt", &dataSet);
	
	readDataDUMMY(&dataSet, N,D);
	//printDataSet(&dataSet);

	
	/*
	int K = 3;
	nnPoint** KNN;
	knn(&dataSet,  N,D,K, &KNN);

	for (i = 0; i<N; i++){
		printf("\n");
		int k;

		for(k=0; k<K; k++)
			printf("%d ", (KNN[i][k].dpoint->label));

		for(k=0; k<K; k++)
			printf("%d ", (KNN[i][k].dist));

		/*
		printDataPoint(*(KNN[i][0].dpoint), D);
		printDataPoint(*(KNN[i][1].dpoint), D);
		printDataPoint(*(KNN[i][2].dpoint), D);
		*/
		/*
	}
		printf("\n");
	*/
	test_distance_matrix();

	return 0;
}


/**
	Some Simple Benchmarking for the parallel calculation of the Distance Matric  
*/
void test_distance_matrix(){

	int i;
	double  seq_time;

	nnPoint ** distMatrix;

	gettimeofday (&startwtime, NULL);
	{
		distance_matrix_OMP(&dataSet, &distMatrix);
	}	
	gettimeofday (&endwtime, NULL);
 	seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);
	printf("\n Parallel Time: %f\n", seq_time);
	

	printf("\n*** Distance Matrix OMP ***\n");
	for(i=0; i<N; i++){
		int j;
		printf("\n");
		for(j=0; j<N; j++)
			printf("%lf ", distMatrix[i][j].dist);
	}
	

	for(i=0; i<N; i++)
		free(distMatrix[i]);
	free(distMatrix);


	gettimeofday (&startwtime, NULL);
	{
		distance_matrix_SEQ(&dataSet, &distMatrix);
	}
	gettimeofday (&endwtime, NULL);
 	seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);
	printf("\n Serial Time: %f\n", seq_time);

	printf("\n*** Distance Matrix SERIAL ***\n");
	for(i=0; i<N; i++){
		int j;
		printf("\n");
		for(j=0; j<N; j++)
			printf("%lf ", distMatrix[i][j].dist);
	}
		
	for(i=0; i<N; i++)
		free(distMatrix[i]);
	free(distMatrix);


}

