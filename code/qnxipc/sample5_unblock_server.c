////////////////////////////////////////////////////////////////////////////////
// unblock_server.c
//
// 
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>

#include "msg_def.h"
#include <unistd.h>

int calculate_checksum(char *text);

int main(void)
{
	int rcvid;
	name_attach_t* attach;
	myMessage_t msg;
	int status;
	int checksum;
	struct _msg_info msg_info;

	attach = name_attach(NULL, UNBLOCK_SERVER, 0);
	if (attach == NULL)
	{ //was there an error creating the channel?
		perror("name_attach"); //look up the errno code and print
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		printf("Waiting for a message...\n");
		status = MsgReceive(attach->chid, &msg, sizeof(msg), &msg_info); //PUT CODE HERE to receive msg from client
		if (status == -1)
		{ //was there an error receiving msg?
			perror("MsgReceive"); //look up errno code and print
			exit(EXIT_FAILURE); //give up
		}
		else if (status > 0)
		{ //msg
			rcvid = status;
			switch (msg.message.msg_type)
			{
			case _IO_CONNECT: //name_open() within the client may send this
				printf("received an _IO_CONNECT msg\n");
				if (-1 == MsgReply(rcvid, EOK, NULL, 0))
					perror("MsgReply");
				continue;
			case CKSUM_MSG_TYPE:
				printf("Received a checksum request msg, rcvid: %d\n", rcvid);
				printf("Calculating checksum, but I'm not going to reply....\n");
				checksum = calculate_checksum(msg.message.string_to_cksum);
				//					status = MsgReply(rcvid, &checksum, sizeof(checksum) );
				//					if(-1 == status) {
				//						perror("MsgReply");
				//						exit(EXIT_FAILURE);
				//					}

				continue;
			default:
				if (-1 == MsgError(rcvid, ENOSYS))
					perror("MsgError");
				continue;
			}
		}
		else if (status == 0)
		{ //pulse
			switch (msg.pulse.code)
			{
			case _PULSE_CODE_DISCONNECT:
				printf("Received disconnect pulse\n");
				if (-1 == ConnectDetach(msg.pulse.scoid))
				{
					perror("ConnectDetach");
				}
				break;
			case _PULSE_CODE_UNBLOCK:
				printf("got an unblock pulse, value is %d\n", msg.pulse.value.sival_int);
				printf("will sleep for 20 seconds, then unblock. \n");
				printf(
						"The Signal Information view should show a pending signal on the client (thread 1)\n");
				sleep(20);
				printf("Unblocking the client now.\n");
				if (-1 == MsgError(msg.pulse.value.sival_int, EINTR))
					perror("MsgError");
				break;
			default:
				printf("unknown pulse received, code = %d\n", msg.pulse.code);
			}
		}
		else
		{
			printf("MsgReceive returned an unexpected value < -1 !!!\n");
		}
	}
	return 0;
}

int calculate_checksum(char *text)
{
	char *c;
	int cksum = 0;

	for (c = text; *c != NULL; c++)
		cksum += *c;
	return cksum;
}

