/*
 * typed_memory.c
 *
 * Demonstrates using the typed memory interface to allocate a block of typed memory.
 * Pass a typed memory name as a parameter, or if no arguments will attempt to allocate from below 4G (x86 system usual)
 */


#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#define MEM_AMT (64*1024)

int main( int argc, char *argv[])
{
	int fd;
	void *ptr;
	off64_t paddr;
	int ret;
	char *memory_name;

	if( argc == 2 )
	{
		memory_name = argv[1];
	}
	else
	{
		memory_name = "/memory/below4G/ram/sysram";
	}

	printf("Attempting to open typed memory object: '%s'\n", memory_name );

	fd = posix_typed_mem_open( memory_name, O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG );
	if( -1 == fd )
	{
		printf("posix_typed_mem_open() failed, errno %d\n", errno );
		return 1;
	}

	ptr = mmap(NULL, MEM_AMT, PROT_READ|PROT_WRITE|PROT_NOCACHE, MAP_SHARED, fd, 0 );
	if( MAP_FAILED == ptr )
	{
		printf("mmap failed, errno %d\n", errno );
		return 1;
	}

	ret = posix_mem_offset64(ptr, 0, &paddr, NULL, NULL );
	if( -1 == ret )
	{
		printf("posix_mem_offset64 failed, errno %d\n", errno );
		return 1;
	}

	printf("Allocated typed memory with a pointer of %p, physical address: %llx\n", ptr, paddr );
	printf("Use pidin or the IDE to examine the address space of this process.\n");

	pause();
    return 0;
}
