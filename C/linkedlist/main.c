#include <stdio.h>
#include "list.h"


int main(void)
{
	List mylist;
	list_init(&mylist, &free);	

	return 0;
}
