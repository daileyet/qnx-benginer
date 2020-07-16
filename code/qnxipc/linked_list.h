#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

struct listNode
{
	struct listNode* next_ptr;
	int scoid;
	char text_description[64];
};

typedef struct listNode listNode_t;

listNode_t* remove_client_from_list(listNode_t* listp, int scoid);
int print_list(listNode_t* list_ptr);
listNode_t* add_client_to_list(listNode_t* listp, int scoid);

#endif //_LINKED_LIST_H_
