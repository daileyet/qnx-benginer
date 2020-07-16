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



	printf("attempting to establish connection with server attach name %s\n",SERVER_NAME);

	//PUT CODE HERE to establish a connection to the server's channel, store the
	//connection id in the variable 'coid'
	coid = name_open(SERVER_NAME,0);

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


	status = MsgSendPulse(coid,-1,_PULSE_CODE_MINAVAIL,10);
	printf("MsgSendPulse return status: %d\n", status);

	name_close(coid);
	return EXIT_SUCCESS;
}

