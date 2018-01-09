
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "data_types.h"


void test_distance_matrix();




struct timeval startwtime, endwtime;

int N,D;	
DataSet dataSet_A,dataSet_B;

int main(int argc, char** argv){
	int i;

	//read_data("./data/formatted_data/mnist_train_svd.txt", &dataSet_A, &N, &D);
	//readData("./data/formatted_data/mnist_train.txt", &dataSet);
	
	read_data_DUMMY(&dataSet_A, 5, 4);
	read_data_DUMMY(&dataSet_B, 5, 4);


	print_dataset(&dataSet_A);	
	print_dataset(&dataSet_B);


//	for (i = 0; i<dataSet.N*dataSet.D; i++)
//		printf("%lf\n", dataSet.data[i]);	
	
	int K = 3;
	nnPoint** KNN;
	knn(&dataSet_A, &dataSet_A, K, &KNN);

	print_knn_matrix(&KNN, dataSet_A.N, K);

	update_knn(&dataSet_A, &dataSet_B, K, &KNN);

	printf("--------------------------------------------------\n");
	print_knn_matrix(&KNN,  dataSet_A.N, K);

/*
	for (i = 0; i<dataSet_A.N; i++){
		printf("\n");
		int k;
		printf("Point %d:\t", dataSet_A.index[i]);	
		for(k=0; k<K; k++)
		{
			printf("[%d %lf]\t", KNN[i][k].index, KNN[i][k].dist);
		}
		printf("\n");
	}
	*/
	




	printf("\n\n\n----------------\n");
	nnPoint *KNNMerged;
	mergesort_nnpoint_arrays(&(KNN[0]), &(KNN[4]), K, &KNNMerged);
	int k;
	for(k=0; k<K; k++)
	{
		printf("[%d %lf]\t", KNNMerged[k].index, KNNMerged[k].dist);
	}

	//test_distance_matrix();

	return 0;
}


/**
	Some Simple Benchmarking for the parallel calculation of the Distance Matric  
*/
void test_distance_matrix(){

	int i;
	double  seq_time;

	nnPoint ** distMatrixOMP;

	gettimeofday (&startwtime, NULL);
	{
		distance_matrix_OMP(&dataSet_A, &dataSet_B, &distMatrixOMP);
	}	
	gettimeofday (&endwtime, NULL);
 	seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);
	printf("\n Parallel Time: %f\n", seq_time);
	
	printf("\n*** Distance Matrix OMP ***\n");
	for(i=0; i<dataSet_A.N; i++){
		int j;
		printf("\n");
		for(j=0; j<dataSet_B.N; j++)
			printf("%lf ", distMatrixOMP[i][j].dist);
	}
	
	for(i=0; i<dataSet_A.N; i++)
		free(distMatrixOMP[i]);
	free(distMatrixOMP);


	nnPoint ** distMatrixSEQ;
	gettimeofday (&startwtime, NULL);
	{
		distance_matrix_SEQ(&dataSet_A, &dataSet_B, &distMatrixSEQ);
	}
	gettimeofday (&endwtime, NULL);
 	seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);
	printf("\n Serial Time: %f\n", seq_time);

	printf("\n*** Distance Matrix SERIAL ***\n");
	for(i=0; i<dataSet_A.N; i++){
		int j;
		printf("\n");
		for(j=0; j<dataSet_B.N; j++)
			printf("%lf ", distMatrixSEQ[i][j].dist);
	}



	for(i=0; i<dataSet_A.N; i++)
		free(distMatrixSEQ[i]);
	free(distMatrixSEQ);
/*
	for(i=0; i<dataSet_A.N; i++){
		int j;
		for(j=0; j<dataSet_B.N; j++)
		{
			double temp = distMatrixOMP[i][j].dist - distMatrixSEQ[i][j].dist;
			if(temp!=0)
			{
				printf("\n\nERROR IN DIST MATRIX\n");
				break;
			}

		}
	}
		
	*/

	printf("\n");
}

