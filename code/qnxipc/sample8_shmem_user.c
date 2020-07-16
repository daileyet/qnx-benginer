/*
 *  shmemuser.c
 *
 *  This module demonstrates shared memory and semaphores
 *  by opening some shared memory, waiting on a semaphore (whose
 *  semaphore structure is in the shared memory) and then printing out
 *  what it finds in the shared memory.
 *
 *  This one is meant to be run in tandem with shmemcreator.c.
 *
 *  Run it as: shmemuser shared_memory_object_name
 *  Example: shmemuser /wally
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

/* shmem.h contains the structure that is overlaid on the shared memory */
#include "shmem.h"

/*
 *  our global variables.
 */

char *progname = "shmem_user";

int main(int argc, char *argv[])
{
	int fd;
	shmem_t *ptr;

	if (argc != 2)
	{
		printf("ERROR: use: shmem_user shared_memory_object_name\n");
		printf("Example: shmem_user /wally\n");
		exit(EXIT_FAILURE);
	}

	if (*argv[1] != '/')
	{
		printf("ERROR: the shared memory name should start with a leading '/' character\n");
		exit(EXIT_FAILURE);
	}

	/* open the shared memory object */

	fd = shm_open(argv[1], O_RDWR, 0);
	if (fd == -1)
	{
		perror("shm_open");
		exit(EXIT_FAILURE);
	}

	/* get a pointer to a piece of the shared memory */

	ptr = mmap(0, sizeof(shmem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED)
	{
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	/* don't need fd anymore */
	close(fd);

	/* wait on the semaphore */

	printf(
			"%s: Waiting on the semaphore. \n Look at state in System information perspective.  This process should be SEM blocked.\n",
			progname);

	if (-1 == sem_wait(&ptr->semaphore))
	{
		perror("sem_wait");
	}

	printf("%s: The shared memory contains '%s'\n", progname, ptr->text);

	if (-1 == munmap(ptr, sizeof(shmem_t)))
	{
		perror("munmap");
	}


	return EXIT_SUCCESS;
}
