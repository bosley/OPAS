#include <stdio.h>
#include "list.h"


int main(void)
{
	List mylist;
	list_init(&mylist, &free);	

	int the_data = 7;


	if( -1 == list_ins_next(&mylist, ListElement, (void*)data) );
		return -1;

	printf("Hola");
	return 0;
}
