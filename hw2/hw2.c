/*
 2020-2 [OPERATING SYSTEMS] Assignment #2
 JuYoung Oh / 20161872 / Electrical & Electronics Engineering
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

/*Define constants*/
#define THREAD_NUM 2
#define MAX_THREAD_NAME_LENGTH 16
#define ROUND 100

/*Declaration of functions*/
void* producer_function(void* arg);
void* consumer_function(void* arg);

/*Global varaible*/
int gNumber = -1;

int main(){
	pthread_t threads[THREAD_NUM];
	char thread_name[THREAD_NUM][MAX_THREAD_NAME_LENGTH] = {"producer", "consumer"};
	char thread_name_got[THREAD_NUM][MAX_THREAD_NAME_LENGTH];
	int status, res, i = 0;
	int* gNumber_result[THREAD_NUM]; // For producer, consumer each

	void* (*fp[THREAD_NUM])(void*) = {producer_function, consumer_function};	

	for(; i < THREAD_NUM; i++){
		status = pthread_create(&threads[i], NULL, fp[i], NULL); // producer, consumer
		
		if(status != 0){
			printf("Thread Creation Error with error code %d\n", status);
			return -1;
		}
		pthread_setname_np(threads[i], thread_name[i]); // set thread name
	}

	for(i = 0; i< THREAD_NUM; i++)
		pthread_getname_np(threads[i], thread_name_got[i], MAX_THREAD_NAME_LENGTH); // read thread name

	for(i = 0; i < THREAD_NUM; i++){
		res = pthread_join(threads[i], &gNumber_result[i]);

		if(res != 0){
			printf("Thread join failed with error code%d\n", res);
			return -1;
		}
		else
			printf("Thread [%s] finished\n", thread_name_got[i]);
	}
	
	if(gNumber_result[0] == gNumber_result[1])
		printf("\nSuccess\nproducer sum: %d, consumer sum: %d\n", (int)gNumber_result[0], (int)gNumber_result[1]);
	else
		printf("\nFail\nproducer sum: %d, Consumer sum: %d\n", (int)gNumber_result[0], (int)gNumber_result[1]);
}

/*Definition of functions*/
void* producer_function(void* arg){
	int i = 0;
	int sum = 0;

	srand(time(NULL));

	for(; i < ROUND; i++){
		if(gNumber == -1){
			int rand_int = rand() % 100;
			sum += rand_int;
			gNumber = rand_int;
			printf("[%d]Generated random int: %d\n", i, rand_int);
		}
		else
			i--;
	}
	pthread_exit((void*)sum);
}

void* consumer_function(void* arg){
	int i = 0;
	int sum = 0;
	for(; i < ROUND; i++){
		if(gNumber != -1){
			printf("[%d]Consumer readout: %d\n", i, gNumber);
			sum += gNumber;
			gNumber = -1;
		}
		else
			i--;
	}
	pthread_exit((void*)sum);
}

