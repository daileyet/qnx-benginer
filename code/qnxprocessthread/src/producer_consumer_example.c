/*
 * producer_consumer_example.c
 *
 *  Created on: Jul 10, 2020
 *      Author: DAD2SZH
 */

#include "producer_consumer_example.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

volatile int ready = 0;

pthread_mutex_t mutex_pc = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_pc = PTHREAD_COND_INITIALIZER;

int producer_consumer_example() {

	// create consumer thread
	pthread_create(NULL, NULL, consume, NULL);
	// create producer thread
	pthread_create(NULL, NULL, produce, NULL);

	// sleep main thread
	sleep(30);
	return 0;
}

void* consume(void *arg) {
	while (1) {
		pthread_mutex_lock(&mutex_pc);
		while (ready == 0) // NOTE: should use while not use if
		{
			pthread_cond_wait(&cond_pc, &mutex_pc);
		}
		printf("Go to do consume...\n");
		ready = 0;
		delay(300);
		printf("Finish consuming work\n");
		pthread_cond_signal(&cond_pc);
		pthread_mutex_unlock(&mutex_pc);
	}
}

void* produce(void *arg) {
	while (1) {
		pthread_mutex_lock(&mutex_pc);
		while (ready == 1) {
			pthread_cond_wait(&cond_pc, &mutex_pc);
		}
		printf("Go to do produce...\n");
		ready = 1;
		delay(300);
		printf("Finish producing work\n");
		pthread_cond_signal(&cond_pc);
		pthread_mutex_unlock(&mutex_pc);
	}
}
