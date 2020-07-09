#include "nomutex.h"
#include <stdio.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <sched.h>
#include <atomic.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define NUMTHREADS      1       // try with one thread, then with 4 to show problem
volatile unsigned var1;
volatile unsigned var2;

char *progname = "nomutex";

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

void do_work() {
	static volatile unsigned var3;

	var3++;
	/* For faster/slower processors, may need to tune this program by
	 * modifying the frequency of this printf -- add/remove a 0
	 */
	if (!(var3 % 10000000))
		printf("%s: thread %d did some work\n", progname, pthread_self());
}

void *
update_thread(void *i) {
	while (1) {
		if (var1 != var2) {
			printf("%s:  thread %d, var1 (%u) is not equal to var2 (%u)!\n",
					progname, pthread_self(), var1, var2);
			var1 = var2;
		}
		/* do some work here */
		do_work();
		var1 += 2;
		var1--;
		var2 += 2;
		var2--;
	}
	return (NULL);
}

int nomutex_sample() {
	int ret;

	pthread_t threadID[NUMTHREADS]; // a place to hold the thread ID's
	pthread_attr_t attrib; // scheduling attributes
	struct sched_param param; // for setting priority
	int i, policy;

	var1 = var2 = 0; /* initialize to known values */

	printf("%s:  starting; creating threads\n", progname);

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

	for (i = 0; i < NUMTHREADS; i++) {
		ret = pthread_create(&threadID[i], &attrib, &update_thread, 0);
		if (ret != EOK) {
			fprintf(stderr, " %s:  pthread_create failed: %s\n", progname,
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

	printf("%s:  stopping; cancelling threads\n", progname);

	for (i = 0; i < NUMTHREADS; i++) {
		pthread_cancel(threadID[i]);
	}

	printf("%s:  all done, var1 is %u, var2 is %u\n", progname, var1, var2);
	fflush(stdout);
	sleep(1);
	exit(0);
}

