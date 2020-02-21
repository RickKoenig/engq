//#define __LISTLIST
#include <stdio.h>
//#include "engine1.h"
#include "u_list.h"

void list_init(struct header *alist)
{
	alist->first=NULL;
	alist->last=NULL;
}

////// add element after key ////////
void list_insert_after_this(struct header *alist,struct node *element,struct node *key)
{
	element->prev=key;
	if (key==NULL) {
		if ((element->next=alist->first)!=0)
			element->next->prev=element;
		else
			alist->last=element;
		alist->first=element;
	} else {
		element->next=key->next;
		if (key==alist->last)
			alist->last=element;
		else
			element->next->prev=element;
		key->next=element;
	}
}

void list_insert_before_this(struct header *alist,struct node *element,struct node *key)
{
	element->next=key;
	if (key==NULL) {
		if ((element->prev=alist->last)!=0)
			element->prev->next=element;
		else
			alist->first=element;
		alist->last=element;
	} else {
		element->prev=key->prev;
		if (key==alist->first)
			alist->first=element;
		else
			element->prev->next=element;
		key->prev=element;
	}
}

/////// remove element //////////
void list_remove_this(struct header *alist,void *ele)
{
	struct node* element = (struct node*) ele;
	if (element == NULL)
		return;
	if	(element->next == NULL)
		alist->last = element->prev;
	else
		element->next->prev = element->prev;

	if	(element->prev == NULL)
		alist->first = element->next;
	else
		element->prev->next = element->next;
}

//// search ////// return -1 if not found
//// feature: if passed a NULL node then routine will return size of list ///
int list_search(struct header *alist,struct node *element)
{
	int cnt=0;
	struct node *node;
	node=alist->first;
	while(node!=element) {
		if (node==NULL)
			return -1;
		node=node->next;
		cnt++;
	}
	return cnt;
}

/// list loop //////
// return element in list given index 0 - n-1 ///
// returns null if list too small //
struct node *list_index(struct header *alist,int index)
{
	struct node *node;
	node=alist->first;
	while(node) {
		if (index-- == 0)
			break;
		node=node->next;
	}
	return node;
}
