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
	char* data;
	name_attach_t *attach;
	attach = name_attach(NULL, IOV_SERVER_NAME, 0);
	if (attach == NULL) {
		perror("name_attach");
		exit(EXIT_FAILURE);
	}
	while (1) {
		printf("Waiting for a message...\n");
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
		if (-1 == rcvid) { //was there an error receiving msg?
			perror("MsgReceive"); //look up errno code and print
			exit(EXIT_FAILURE); //give up
		} else if (rcvid == 0) { // process pulse
			int code = msg.pulse.code;
			switch (code) {
			case _PULSE_CODE_DISCONNECT:
				printf("Received disconnect pulse\n");
				if (-1 == ConnectDetach(msg.pulse.scoid)) {
					perror("ConnectDetach");
				}
				break;
				break;
			case _PULSE_CODE_UNBLOCK:
				printf("Server receive a client wants unblock pulse");
				break;
			default:
				printf("unknown pulse received, code = %d\n", code);
			}
		} else if (rcvid > 0) {
			switch (msg.header.msg_type) {
			case _IO_CONNECT:
				printf("received an _IO_CONNECT msg\n");
				if (MsgReply(rcvid, EOK, NULL, 0) == -1)
					perror("MsgReply");
				break;
			case CKSUM_MSG_TYPE:
				printf(
						"Received a checksum request msg, header says the data is %d bytes\n",
						msg.header.data_size);
				data = malloc(msg.header.data_size);
				if (data == NULL) {
					if (-1 == MsgError(rcvid, ENOMEM)) {
						perror("MsgError");
						exit(EXIT_FAILURE);
					}
				} else {
					status = MsgRead(rcvid, data, msg.header.data_size,
							sizeof(cksum_header_t));
					if (-1 == status) {
						perror("MsgRead");
						exit(EXIT_FAILURE);
					}
					checksum = calculate_checksum(data);
					free(data);
					status = MsgReply(rcvid, EOK, &checksum, sizeof(checksum));
					if (-1 == status) {
						perror("MsgReply");
						exit(EXIT_FAILURE);
					}
				}
				break;
			default:
				if (MsgError(rcvid, ENOSYS) == -1) {
					perror("MsgError");
				}
				break;
			}
		} else {
			printf("Receive returned an unexpected value: %d\n", rcvid);
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

