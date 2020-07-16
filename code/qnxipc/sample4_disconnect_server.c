////////////////////////////////////////////////////////////////////////////////
// disconnect server.c
//
// demonstrates handling the disconnect pulse
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>

#include "msg_def.h"
#include "linked_list.h"

int calculate_checksum(char *text);
listNode_t* add_client_to_list(listNode_t* listp, int scoid);
listNode_t* remove_client_from_list(listNode_t* list_ptr, int scoid);

int main(void) {
	int rcvid;
	name_attach_t* attach;
	myMessage_t msg;
	int status;
	int checksum;
	struct _msg_info msg_info;
	listNode_t* list_ptr = NULL;

	attach = name_attach(NULL, DISCONNECT_SERVER, 0);
	if (NULL == attach) { //was there an error creating the channel?
		perror("name_attach"); //look up the errno code and print
		exit(EXIT_FAILURE);
	}

	while (1) {
		printf("Waiting for a message...\n");
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), &msg_info); //PUT CODE HERE to receive msg from client
		if (-1 == rcvid) { //was there an error receiving msg?
			perror("MsgReceive"); //look up errno code and print
			exit(EXIT_FAILURE); //failure, terminate
		} else if (rcvid > 0) { //msg
			switch (msg.message.msg_type) {
			case _IO_CONNECT: //name_open() within the client may send this
				if (-1 == MsgReply(rcvid, EOK, NULL, 0)) {
					perror("MsgReply");
					break;
				}
				break;
			case CKSUM_MSG_TYPE:
				printf("Message received, client scoid = %x\n", msg_info.scoid);
				list_ptr = add_client_to_list(list_ptr, msg_info.scoid);
				print_list(list_ptr);

				checksum = calculate_checksum(msg.message.string_to_cksum);

				status = MsgReply(rcvid, EOK, &checksum, sizeof(checksum));
				if (-1 == status) {
					perror("MsgReply");
					exit(EXIT_FAILURE);
				}
				break;
			default:
				if (-1 == MsgError(rcvid, ENOSYS)) {
					perror("MsgError");
				}
			}
		} else if (rcvid == 0) { //message
			switch (msg.pulse.code) {
			case _PULSE_CODE_DISCONNECT:
				printf("received disconnect pulse from client, scoid = %x\n",
						msg.pulse.scoid);
				list_ptr = remove_client_from_list(list_ptr, msg.pulse.scoid);
				if (-1 == ConnectDetach(msg.pulse.scoid))
					perror("ConnectDetach");
				print_list(list_ptr);
				break;
			default:
				printf("unknown pulse received, code = %x\n", msg.pulse.code);
			}
		} else {
			printf("received unexpected msg with rcvid < -1 !!!\n");
		}
	}
	return 0;
}

int calculate_checksum(char *text) {
	char *c;
	int cksum = 0;

	for (c = text; *c != NULL; c++)
		cksum += *c;
	return cksum;
}

listNode_t* add_client_to_list(listNode_t* listp, int scoid) {
	listNode_t* newp;

	printf("adding client to list, scoid: %x\n", scoid);

	newp = malloc(sizeof(listNode_t));
	if (newp == NULL) {
		errno = ENOMEM;
		return NULL;
	}
	newp->next_ptr = listp;
	newp->scoid = scoid;
	return newp;
}

int print_list(listNode_t* list_ptr) {
	listNode_t* current_ptr = list_ptr;

	if (list_ptr == NULL) {
		printf("no clients in list\n");
	} else {
		printf("list of currently connected clients: \n");

		while (current_ptr != NULL) {
			printf("client scoid: %x \n", current_ptr->scoid);
			current_ptr = current_ptr->next_ptr;
		}
	}
	printf("\n");

	return 0;
}

listNode_t* remove_client_from_list(listNode_t* listp, int scoid) {
	listNode_t* curp = listp;
	listNode_t* prevp = NULL;

	printf("removing client from list, scoid: %x\n", scoid);

	while ((curp) && curp->scoid != scoid) {
		prevp = curp;
		curp = curp->next_ptr;
	}
	if (curp == NULL) {
		printf("couldn't find client %x\n", scoid);
		return listp;
	}

	if (prevp == NULL) {
		printf("found client at head node\n");
		prevp = curp->next_ptr;
		free(curp);
		return prevp;
	}
	printf("found client at node other than head\n");
	prevp->next_ptr = curp->next_ptr;
	free(curp);
	return listp;
}

