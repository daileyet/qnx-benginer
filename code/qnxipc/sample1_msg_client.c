////////////////////////////////////////////////////////////////////////////////
// client.c
//
// A QNX msg passing client.  It's purpose is to send a string of text to a
// server.  The server calculates a checksum and replies back with it.  The client
// expects the reply to come back as an int
//
// This program program must be started with commandline args.  
// See  if(argc != 4) below
//
// To complete the exercise, put in the code, as explained in the comments below
// Look up function arguments in the course book or the QNX documentation.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>     // #define for ND_LOCAL_NODE is in here
#include "msg_def.h"

int main(int argc, char* argv[]) {
	int coid; //Connection ID to server
	cksum_msg_t msg;
	int incoming_checksum; //space for server's reply
	int status; //status return value used for ConnectAttach and MsgSend
	int server_pid; //server's process ID
	int server_chid; //server's channel ID

	if (4 != argc) {
		printf(
				"ERROR: This program must be started with commandline arguments, for example:\n\n");
		printf("   client 482834 1 abcdefghi    \n\n");
		printf(" 1st arg(482834): server's pid\n");
		printf(" 2nd arg(1): server's chid\n");
		printf(" 3rd arg(abcdefghi): string to send to server\n"); //to make it 
		//easy, let's not bother handling spaces
		exit(EXIT_FAILURE);
	}

	server_pid = atoi(argv[1]);
	server_chid = atoi(argv[2]);

	printf("attempting to establish connection with server pid: %d, chid %d\n",
			server_pid, server_chid);

	//PUT CODE HERE to establish a connection to the server's channel, store the
	//connection id in the variable 'coid'
	coid = ConnectAttach(ND_LOCAL_NODE, server_pid, server_chid,
			_NTO_SIDE_CHANNEL, 0);

	if (-1 == coid) { //was there an error attaching to server?
		perror("ConnectAttach"); //look up error code and print
		exit(EXIT_FAILURE);
	}

	msg.msg_type = CKSUM_MSG_TYPE;
	strcpy(msg.string_to_cksum, argv[3]);
	printf("Sending string: %s\n", msg.string_to_cksum);

	//PUT CODE HERE to send message to server and get the reply
	status = MsgSend(coid, &msg, sizeof(msg), &incoming_checksum,
			sizeof(incoming_checksum));
	if (-1 == status) { //was there an error sending to server?
		perror("MsgSend");
		exit(EXIT_FAILURE);
	}

	printf("received checksum=%d from server\n", incoming_checksum);
	printf("MsgSend return status: %d\n", status);

	return EXIT_SUCCESS;
}

