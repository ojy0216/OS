/*
2020-2 [OPERATING SYSTEMS] Assignment #3
JuYoung Oh / 20161872 / Electrical & Electronics Engineering
*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

#define ROUND 10
#define BANK_INIT_BALANCE 100
#define CLIENT_NUM 2
// complile option with "-no-pie"

unsigned int bank_balance = 100;
unsigned int client_debt[CLIENT_NUM] = {0, 0};

sem_t bin_sem;

void enter_region(){
	sem_wait(&bin_sem);
}

void leave_region(){
	sem_post(&bin_sem);
}

void loan(unsigned int money, int client_id){
	enter_region();
	int d = rand () % 1000000;
	if(money <= bank_balance){
		printf("Client[%d] Loan request: %u 만원\n", client_id, money);
		usleep(d);
		bank_balance -= money;
		client_debt[client_id - 1] += money;
		printf("[Loan Complete] Client[%d] remaining debt: %u 만원\n", client_id, client_debt[client_id - 1]);
	}
	else
		printf("Client[%d] Loan request: %u 만원 rejected(Not enough bank balance)\n",client_id, money);	
	printf("Bank balance: %u 만원\n\n", bank_balance);
	leave_region();
}

void repay(unsigned int money, int client_id){
	enter_region();
	int d = rand () % 1000000;
	if(money <= client_debt[client_id - 1]){
		printf("Client[%d] Repayment request: %u 만원\n", client_id, money);
		usleep(d);
		bank_balance += money;
		client_debt[client_id - 1] -= money;
		printf("[Repayment Complete] Client[%d] remaining debt: %u 만원\n", client_id, client_debt[client_id - 1]);
	}
	else{
		printf("Client[%d] Repayment request: %u 만원, repaied %u 만원\n", client_id, money, client_debt[client_id - 1]);
		printf("No remaining debt, difference %u return to Client[%d]\n", money - client_debt[client_id - 1], client_id);
		bank_balance += client_debt[client_id - 1];
		client_debt[client_id - 1] = 0;
	}
	printf("Bank balance: %u 만원\n\n", bank_balance);
	leave_region();
}

void use_money(){
	int d = rand () % 1000000;
	usleep(d);
}

static void* c1(int client_id){
	loan(10, client_id);

	use_money();

	loan(30, client_id);

	use_money();

	loan(50, client_id);

	use_money();

	repay(40, client_id);

	use_money();

	repay(20, client_id);

	use_money();

	loan(20, client_id);

	use_money();

	repay(50, client_id);
	
	return NULL;
}

static void* c2(int client_id){
	loan(30, client_id);

	use_money();

	repay(20, client_id);

	use_money();

	loan(50, client_id);

	use_money();

	repay(50, client_id);

	use_money();

	loan(50, client_id);

	use_money();

	repay(60, client_id);

	return NULL;
}

int main(){
	int rc;
	sem_init(&bin_sem, 0, 1);

	pthread_t t1, t2;

	rc = pthread_create(&t1, NULL, c1, 1);
	if(rc != 0){
		fprintf(stderr, "pthread c1 failed\n");
		return EXIT_FAILURE;
	}

	rc = pthread_create(&t2, NULL, c2, 2);
	if(rc != 0){
		fprintf(stderr, "pthread c2 failed\n");
		return EXIT_FAILURE;
	}

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	puts("All threads finished\n");

	sem_destroy(&bin_sem);

	return 0;
}

