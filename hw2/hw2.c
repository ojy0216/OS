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
#define ROUND 100

/*Declaration of functions*/
void* producer(void* arg);
void* consumer(void* arg);

/*Global varaible*/
int gNumber = -1;

int main(){
	pthread_t threads[THREAD_NUM]; // producer, consumer
	int i = 0;
	int* gNumber_result[THREAD_NUM]; // For producer, consumer each

	void* (*fp[THREAD_NUM])(void*) = {producer, consumer};	

	for(; i < THREAD_NUM; i++){
		if(pthread_create(&threads[i], NULL, fp[i], NULL) != 0){
			printf("Thread Creation Error\n");
			return -1;
		}
	}

	for(i = 0; i < THREAD_NUM; i++){
		if(pthread_join(threads[i], &gNumber_result[i]) != 0){
			printf("Thread join failed\n");
			return -1;
		}
	}
	
	if(gNumber_result[0] == gNumber_result[1])
		printf("\nSuccess\nproducer sum: %d, consumer sum: %d\n", (int)gNumber_result[0], (int)gNumber_result[1]);
	else
		printf("\nFail\nproducer sum: %d, Consumer sum: %d\n", (int)gNumber_result[0], (int)gNumber_result[1]);
}

/*Definition of functions*/
void* producer(void* arg){
	int i = 0, sum = 0, rand_int;

	srand(time(NULL));

	for(; i < ROUND; i++){
		if(gNumber == -1){
			rand_int = rand() % 100;
			sum += rand_int;
			printf("[%d]Producer generated: %d\n", i, rand_int);
			gNumber = rand_int;
		}
		else
			i--;
	}
	pthread_exit((void*)sum);
}

void* consumer(void* arg){
	int i = 0, sum = 0;
	for(; i < ROUND; i++){
		if(gNumber != -1){
			printf("[%d]Consumer read: %d\n", i, gNumber);
			sum += gNumber;
			gNumber = -1;
		}
		else
			i--;
	}
	pthread_exit((void*)sum);
}
