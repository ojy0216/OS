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

#define THREAD_NUM 2
#define MAX_THREAD_NAME_LENGTH 16
#define ROUND 100

void* producer_function(void* arg);
void* consumer_function(void* arg);

int gNumber;

int main(){
	int res;
	pthread_t threads[THREAD_NUM];
	char thread_name[THREAD_NUM][MAX_THREAD_NAME_LENGTH] = {"producer", "consumer"};
	char thread_name_got[THREAD_NUM][MAX_THREAD_NAME_LENGTH];
	int status, i = 0;
	int* gNumber_result[THREAD_NUM];
	int error_code;

	void* (*fp[THREAD_NUM])(void*);	
	fp[0] = producer_function;
	fp[1] = consumer_function;

	for(; i < THREAD_NUM; i++){
		status = pthread_create(&threads[i], NULL, fp[i], NULL);
		
		if(status != 0){
			printf("Thread Creation Error with error code %d\n", status);
			return -1;
		}
		else
			pthread_setname_np(threads[i], thread_name[i]);
	}

	for(i = 0; i < THREAD_NUM; i++){
		pthread_getname_np(threads[i], thread_name_got[i], MAX_THREAD_NAME_LENGTH);

		res = pthread_join(threads[i], &gNumber_result[i]);

		if(res != 0){
			printf("Thread join failed with error code%d\n", res);
			return -1;
		}
	}

	for(i = 0; i < THREAD_NUM; i++)
		printf("Thread with name : [%s] finished\n", thread_name_got[i]);

	if(gNumber_result[0] == gNumber_result[1])
		printf("Success\n");
		/*printf("%d\n", gNumber_result[0]);*/
	else
		printf("Fail\n");
}

void* producer_function(void* arg){
	int i = 0;
	int sum = 0;

	srand(time(NULL));

	for(; i < ROUND; i++){
		int rand_int = rand() % 100;
		sum += rand_int;
		gNumber = rand_int;
		printf("[%d]Generated random int: %d\n", i, rand_int);
	}
	pthread_exit((void*)sum);
}

void* consumer_function(void* arg){
	int i = 0;
	int sum = 0;
	for(; i < ROUND; i++){
		printf("[%d]Consumer readout: %d\n", i, gNumber);
		sum += gNumber;
	}
	pthread_exit((void*)sum);
}

