#include <semaphore.h>

#define MAX_TEXT_LEN    100

typedef struct
{
	sem_t semaphore;
	char text[MAX_TEXT_LEN + 1];
} shmem_t;
