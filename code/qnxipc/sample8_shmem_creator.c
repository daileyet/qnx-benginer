/*
 *  shmemcreator.c
 *
 *  This module demonstrates shared memory and semaphores
 *  by creating some shared memory and putting something in it (including
 *  a semaphore structure).  It then posts to the semaphore to tell other
 *  processes that it is okay to read from the shared memory.
 *
 *  This one is meant to be run in tandem with shmemuser.c.
 *
 *  Run it as: shmemcreator shared_memory_object_name
 *  Example: shmemcreator /wally
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

char *progname = "shmem_creator";

int main(int argc, char *argv[])
{
	char *text = "Text by shmem_creator.c";
	int fd;
	shmem_t *ptr;
	int ret;

	if (argc != 2)
	{
		printf("ERROR: use: shmem_creator shared_memory_object_name\n");
		printf("Example: shmem_creator /wally\n");
		exit(EXIT_FAILURE);
	}
	if (*argv[1] != '/')
	{
		printf("ERROR: the shared memory name should start with a leading '/' character\n");
		exit(EXIT_FAILURE);
	}

	/* create the shared memory object */

	fd = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, 0666);
	if (fd == -1)
	{
		perror("shm_open()");
		exit(EXIT_FAILURE);
	}

	/* set the size of the shared memory object */

	ret = ftruncate(fd, sizeof(shmem_t));
	if (-1 == ret)
	{
		perror("ftruncate");
		exit(EXIT_FAILURE);
	}

	/* get a pointer to a piece of the shared memory */

	ptr = mmap(0, sizeof(shmem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (MAP_FAILED == ptr)
	{
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	/* don't need fd anymore, so close it */
	close(fd);

	/*
	 * initialize the semaphore
	 * The 1 means it can be shared between processes.  The 0 is the
	 * count, 0 meaning sem_wait() calls will block until a sem_post()
	 * is done.
	 */
	if (-1 == sem_init(&ptr->semaphore, 1, 0))
	{
		perror("sem_init");
		munmap(ptr, sizeof(shmem_t));
		shm_unlink(argv[1]);
		exit(EXIT_FAILURE);
	}

	strcpy(ptr->text, text); /* write to the shared memory */

	printf("%s: Shared memory created and semaphore initialized to 0.\n"
		"%s: Wrote text '%s' to shared memory.\n"
		"%s: Sleeping for 30 seconds.  While this program is sleeping\n"
		"%s: run 'shmem_user %s'.\n", progname, progname, ptr->text, progname, progname, argv[1]);
	sleep(30);

	printf("%s: Woke up.  Now posting to the semaphore.\n", progname);
	if (-1 == sem_post(&ptr->semaphore))
	{
		perror("sem_post");
	}

	/* another delay for people to look at things */
	sleep(300);

	if (-1 == munmap(ptr, sizeof(shmem_t)))
	{
		perror("munmap");
	}
	if (-1 == shm_unlink(argv[1]))
	{
		perror("shm_unlink");
	}

	return EXIT_SUCCESS;
}
