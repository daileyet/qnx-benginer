#include "spawn_example.h"

int spawn_example(int argc, char **argv)
{
	int pid;
	int status;
	sigset_t set;

	// create a new child process
	pid = spawnl(P_NOWAIT, "/usr/bin/sleep", "sleep", "30", NULL );
	if (-1 == pid)
	{
		perror("spawnl()");
		exit(EXIT_FAILURE);
	}

	printf("child pid is %d\n", pid);
	printf("View the process list in the IDE or at the command line.\n");
	printf("In the IDE Target Navigator menu try group->by PID family\n");
	printf("With pidin, try 'pidin family' to get parent/child information.\n");

	// initial sigset  data struct
	if (-1 == sigemptyset(&set))
	{
		perror("sigemptyset");
		exit(EXIT_FAILURE);
	}

	// add interesting signal in set
	if (-1 == sigaddset(&set, SIGCHLD ))
	{
		perror("sigaddset");
		exit(EXIT_FAILURE);
	}

	// block here to wait a interesting signal
	if (-1 == sigwaitinfo(&set, NULL )) // block until a SIGCHLD hits, that is, until the child process dies
	{
		perror("sigwaitinfo");
		exit(EXIT_FAILURE);
	}

	printf("Child has died, pidin should now show it as a zombie\n");
	sleep(30);

	// wait for child process state to be terminated
	pid = wait(&status); // get the status of the dead child, and
	if (-1 == pid)
	{
		perror("wait");
		exit(EXIT_FAILURE);
	}
	printf("child process: %d, died with status %x\n", pid, status);
	printf("Zombie is now gone as we've waited on the child process.\n");
	sleep(30);

	return 0;
}

