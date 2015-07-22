#include <stdlib.h>
#include <string.h>

#include "list.h"

void list_init(List *list, void (*destroy)(void *data))
{
	list->size = 0;
	list->destroy = destroy;
	list->head = NULL;
	list->tail = NULL;
	return;
}

void list_destroy(List *list)
{
	void *data;	
	while(list_size(list) > 0)
	{
		if( list_rem_next(list, NULL, (void**)&data) == 0 &&
		    list->destroy != NULL )
		{
			list->destroy(data);
		} 
	}
	memset(list, 0, sizeof(List));
	return;
}

int list_ins_next(List *list, ListElement * el, const void *data)
{
	ListElement *new_element;
	
	// Allocate memory
	if ((new_element = (ListElement*)malloc(sizeof(ListElement))) == NULL)
		return -1;

	new_element->data = (void*)data;

	// Insert as head	
	if(el == NULL)
	{
		if(list_size(list) == 0)
			list->tail = new_element;
		new_element->next = list->head;
		list->head = new_element;
	} 
	// Insert as not-head
	else
	{
		if (el->next == NULL)
			list->tail = new_element;

		new_element->next = el->next;
		el->next = new_element;	
	}
	list->size++;
	return 0;
}

int list_rem_next(List *list, ListElement *el, void **data)
{
	// If empty, can't remove 
	if(list_size(list) == 0)
		return -1;

	ListElement *old_element;
	
	if(el == NULL)
	{
		*data = list->head->data;
		old_element = list->head;
		list->head = list->head->next;

		if(list_size(list) == 1)
			list->tail = NULL;
	}
	else
	{
		// [HANDLE REMOVAL FROM SOMEHWERE THAT ISNT THE HEAD]	
		if(el->next == NULL)
			return -1;
		
		*data = el->next->data;
		old_element = el->next;
		el->next = el->next->next;
		
		if(el->next == NULL)
			list->tail = el;
	}

	free(old_element);
	list->size--;
	return 0;
}



