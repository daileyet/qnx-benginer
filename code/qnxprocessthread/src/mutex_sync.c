/*
 * mutex_sync.c
 *
 */

#include "mutex_sync.h"

#include <stdio.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define NUMTHREADS2      4

volatile unsigned var1;
volatile unsigned var2;
char *progname2 = "mutex_sync";

// mark as "not in use" and "to be initialized the first time that it is used"
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;


void do_work2() {
	static volatile unsigned var3;

	var3++;
	/* For faster/slower processors, may need to tune this program by
	 * modifying the frequency of this printf -- add/remove a 0
	 */
	if (!(var3 % 10000000))
		printf("%s: thread %d did some work\n", progname2, pthread_self());
}

/*
 *  the actual thread.
 *
 *  The thread ensures that var1 == var2.  If this is not the
 *  case, the thread sets var1 = var2, and prints a message.
 *
 *  Var1 and Var2 are incremented.
 *
 *  Looking at the source, if there were no "synchronization" problems,
 *  then var1 would always be equal to var2.  Run this program and see
 *  what the actual result is...
 */
void *
update_thread2(void *i) {
	while (1) {
		int isEqual = 0;
		// lock here for read var1 and var2
		pthread_mutex_lock(&mymutex);
		isEqual = (var1 != var2) ;
		pthread_mutex_unlock(&mymutex);


		if (isEqual) {
			printf("%s:  thread %d, var1 (%u) is not equal to var2 (%u)!\n",
					progname2, pthread_self(), var1, var2);
			var1 = var2;
		}


		/* do some work here */
		do_work2();

		// lock here for write var1 and var2
		pthread_mutex_lock(&mymutex);
		var1 += 2;
		var1--;
		var2 += 2;
		var2--;
		pthread_mutex_unlock(&mymutex);

	}
	return (NULL);
}

int mutex_sync_sample() {
	int ret;

	pthread_t threadID[NUMTHREADS2]; // a place to hold the thread ID's
	pthread_attr_t attrib; // scheduling attributes
	struct sched_param param; // for setting priority
	int i, policy;

	var1 = var2 = 0; /* initialize to known values */

	printf("%s:  starting; creating threads\n", progname2);

	/*
	 *  we want to create the new threads using Round Robin
	 *  scheduling, and a lowered priority, so set up a thread
	 *  attributes structure.  We use a lower priority since these
	 *  threads will be hogging the CPU
	 */

	pthread_getschedparam(pthread_self(), &policy, &param);

	pthread_attr_init(&attrib);
	pthread_attr_setinheritsched(&attrib, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attrib, SCHED_RR);
	param.sched_priority -= 2; // drop priority a couple levels
	pthread_attr_setschedparam(&attrib, &param);

	/*
	 *  create the threads.  As soon as each pthread_create
	 *  call is done, the thread has been started.
	 */

	for (i = 0; i < NUMTHREADS2; i++) {
		ret = pthread_create(&threadID[i], &attrib, &update_thread2, 0);
		if (ret != EOK) {
			fprintf(stderr, " %s:  pthread_create failed: %s\n", progname2,
					strerror(ret));
			exit(EXIT_FAILURE);
		}
	}

	/*
	 *  let the other threads run for a while
	 */

	sleep(15);

	/*
	 *  and then kill them all
	 */

	printf("%s:  stopping; cancelling threads\n", progname2);

	for (i = 0; i < NUMTHREADS2; i++) {
		pthread_cancel(threadID[i]);
	}

	printf("%s:  all done, var1 is %u, var2 is %u\n", progname2, var1, var2);
	fflush(stdout);
	sleep(1);
	exit(0);
}
