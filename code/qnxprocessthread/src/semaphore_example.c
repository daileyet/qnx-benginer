/*
 *  semex.c
 *
 *  This module demonstrates POSIX semaphores.
 *
 *  Operation:
 *      A counting semaphore is created, primed with 0 counts.
 *      Five consumer threads are started, each trying to obtain
 *      the semaphore.
 *      A producer thread is created, which periodically posts
 *      the semaphore, unblocking one of the consumer threads.
 *
 */
#include "semaphore_example.h"
#include <stdio.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>

/*
 *  our global variables, and forward references
 */

sem_t *mySemaphore;

void *producer_semaphore(void *);
void *consumer_semaphore(void *);

char *progname_semaphore = "semex";

#define SEM_NAME "/Semex"

int semaphore_example() {
	// #define  NAMED
#ifdef  NAMED
	mySemaphore = sem_open (SEM_NAME, O_CREAT, S_IRWXU, 0);
	/* not sharing with other process, so immediately unlink */
	if (mySemaphore == SEM_FAILED)
	{
		perror("sem_open()");
		exit(EXIT_FAILURE);
	}
	if (-1 == sem_unlink( SEM_NAME ))
	perror("sem_unlink");
#else   // NAMED
	mySemaphore = malloc(sizeof(sem_t));
	if (-1 == sem_init(mySemaphore, 1, 0)) {
		perror("sem_init()");
		exit(EXIT_FAILURE);
	}
#endif  // NAMED
	for (int i = 0; i < 5; i++) {
		if ((pthread_create(NULL, NULL, consumer_semaphore, (void *) i)) != EOK) {
			fprintf(stderr, "%s: a consumer pthread_create failed.\n",
					progname_semaphore);
			exit(EXIT_FAILURE);
		}
	}

	if ((pthread_create(NULL, NULL, producer_semaphore, (void *) 1)) != EOK) {
		fprintf(stderr, "%s: producer pthread_create failed.\n",
				progname_semaphore);
		exit(EXIT_FAILURE);
	}

	sleep(20); // let the threads run
	printf("%s:  main, exiting\n", progname_semaphore);
	return 0;
}

/*
 *  producer
 */
void *
producer_semaphore(void *i) {
	while (1) {
		sleep(1);
		printf("%s:  (producer %d), posted semaphore\n", progname_semaphore,
				(int) i);
		if (-1 == sem_post(mySemaphore)) {
			perror("sem_post");
			exit(EXIT_FAILURE);
		}
	}
	return (NULL);
}

/*
 *  consumer
 */
void *
consumer_semaphore(void *i) {
	while (1) {
		if (-1 == sem_wait(mySemaphore)) {
			perror("sem_wait");
			exit(EXIT_FAILURE);
		}
		printf("%s:  (consumer %d) got semaphore\n", progname_semaphore,
				(int) i);
	}
	return (NULL);
}
