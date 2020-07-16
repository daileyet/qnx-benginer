////////////////////////////////////////////////////////////////////////////////
// unblock_client.c
//
// 
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>     // #define for ND_LOCAL_NODE is in here
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#include "msg_def.h"

int main(int argc, char* argv[])
{
	int coid; //Connection ID to server
	cksum_msg_t msg;
	int incoming_checksum; //space for server's reply
	int status; //status return value used for ConnectAttach and MsgSend


	if (2 != argc)
	{
		printf("ERROR: This program must be started with a command-line arg, for example:\n\n");
		printf("   disconnect_client abcdefghi    \n\n");
		printf(" where 1st arg(abcdefghi) is string to send to server\n"); //to make it 
		//easy, let's not bother handling spaces
		exit(EXIT_FAILURE);
	}

	coid = name_open(UNBLOCK_SERVER, 0);
	if (-1 == coid)
	{ //was there an error attaching to server?
		perror("name_open"); //look up error code and print
		exit(EXIT_FAILURE);
	}

	msg.msg_type = CKSUM_MSG_TYPE;
	strcpy(msg.string_to_cksum, argv[1]);
	printf("Sending string: %s\n", msg.string_to_cksum);

	status = MsgSend(coid, &msg, sizeof(msg), &incoming_checksum, sizeof(incoming_checksum));//PUT CODE HERE to send message to server and get the reply
	if (-1 == status)
	{ //was there an error sending to server?
		perror("MsgSend");
		exit(EXIT_FAILURE);
	}

	printf("received checksum=%d from server\n", incoming_checksum);
	printf("MsgSend return status: %d\n", status);

	return EXIT_SUCCESS;
}

