////////////////////////////////////////////////////////////////////////////////
// server.c
//
// A QNX msg passing server.  It should receive a string from a client,
// calculate a checksum on it, and reply back the client with the checksum.
//
// The server prints out its pid and chid so that the client can be made aware
// of them.
//
// Using the comments below, put code in to complete the program.  Look up 
// function arguments in the course book or the QNX documentation.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <process.h>
#include <sys/dispatch.h>

#include "msg_def.h"  //layout of msg's should always defined by a struct, here's its definition

int
calculate_checksum(char *text);

int main(void) {
	int rcvid;
	myMessage_t msg;
	int status;
	int checksum;

	name_attach_t *attach;
	attach = name_attach(NULL, SERVER_NAME, 0);

	while (1) {
		//PUT CODE HERE to receive msg from client
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
		if (-1 == rcvid) { //was there an error receiving msg?
			perror("MsgReceive"); //look up errno code and print
			exit(EXIT_FAILURE); //give up
		}
		if (rcvid == 0) { // process pulse
			int code = msg.pulse.code;
			printf("Server receive a pulse, code:%d, value:%d\n",
					code, msg.pulse.value);

			switch (code) {
			case _PULSE_CODE_DISCONNECT:
				printf("Server receive a client disconnected pulse");
				break;
			case _PULSE_CODE_UNBLOCK:
				printf("Server receive a client wants unblock pulse");
				break;
			}
		} else {
			printf("Server receive a message, content: %s\n",
					msg.message.string_to_cksum);
			//PUT CODE HERE to calculate the check sum by calling calculate_checksum()
			checksum = calculate_checksum(msg.message.string_to_cksum);
			//PUT CODE HERE TO reply to client with checksum, store the return status in status
			status = MsgReply(rcvid, EOK, &checksum, sizeof(checksum));
			if (-1 == status) {
				perror("MsgReply");
				exit(EXIT_FAILURE);
			}
		}
	}
	name_detach(attach, 0);
	return 0;
}

int calculate_checksum(char *text) {
	char *c;
	int cksum = 0;

	for (c = text; *c != NULL; c++)
		cksum += *c;
	return cksum;
}

