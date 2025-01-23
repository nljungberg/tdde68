#include "list.h"
#include <stdio.h>
#include <stdlib.h>


int main()
{
	struct list_item root;
	root.value = -1;
	root.next = NULL;
	struct list_item *newroot = malloc(sizeof(struct list_item));
	append(newroot, 10);

	printf("%d\n", newroot->next->value);
	prepend(newroot, 5);

	printf("%d\n", newroot->value);
	//printf("%d\n", newroot->next->next->value);
	input_sorted(newroot, 12);
	printf("%d\n", newroot->next->next->value);
	print(newroot);

	/* Write your test cases here */
	return 0;
}
