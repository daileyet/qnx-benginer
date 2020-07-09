/*
 * nomutext.c
 *
 *  Created on: Jul 9, 2020
 *      Author: DAD2SZH
 */
#include <stdio.h>
#include "spawn_example.h"
#include "nomutex.h"
#include "mutex_sync.h"

int main(int argc, char **argv)
{
	printf("QNX process&thread samples list:.\n");
	printf("1.spawn_example\n");
	printf("2.nomutex_example\n");
	printf("3.mutex_sync_example\n");
	printf("Please pick/enter a sequence number to run:");

	int choice = 0;
	scanf("%d",&choice);

	switch(choice){
	case 1:
		spawn_example(argc,argv);
		break;
	case 2:
		nomutex_sample();
		break;
	case 3:
		mutex_sync_sample();
		break;
	default:
		printf("Invalid choice, exit.\n");
		break;
	}

}


