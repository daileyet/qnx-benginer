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
#include <unistd.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

int main(int argc, char* argv[]) {
	int coid; //Connection ID to server
	cksum_header_t header;
	int incoming_checksum; //space for server's reply
	int status; //status return value used for ConnectAttach and MsgSend
	iov_t siov[2];

	if (2 != argc) {
		printf(
				"ERROR: This program must be started with a command-line arg, for example:\n\n");
		printf("   iov_client abcdefjhi    \n\n");
		printf(
				" where 1st arg(abcdefghi) is the text to be sent to the server to be checksum'd\n");
		exit(EXIT_FAILURE);
	}
	printf("attempting to establish connection with server attach name %s\n",
	IOV_SERVER_NAME);

	//PUT CODE HERE to establish a connection to the server's channel, store the
	//connection id in the variable 'coid'
	coid = name_open(IOV_SERVER_NAME, 0);

	if (-1 == coid) { //was there an error attaching to server?
		perror("ConnectAttach"); //look up error code and print
		exit(EXIT_FAILURE);
	}
	printf("Sending the following text to checksum server: %s\n", argv[1]);

	header.msg_type = CKSUM_MSG_TYPE;
	header.data_size = strlen(argv[1]) + 1;

	SETIOV(&siov[0], &header, sizeof(header));
	SETIOV(&siov[1], argv[1], header.data_size);

	status = MsgSendvs(coid, siov, 2, &incoming_checksum,
			sizeof(incoming_checksum));
	if (-1 == status) { //was there an error sending to server?
		perror("MsgSend");
		exit(EXIT_FAILURE);
	}

	printf("received checksum=%d from server\n", incoming_checksum);
	printf("MsgSend return status: %d\n", status);

	name_close(coid);
	return EXIT_SUCCESS;
}

