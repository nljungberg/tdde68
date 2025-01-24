#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool test_append() {
    struct list_item *root = malloc(sizeof(struct list_item));

	append(root, 100);
	if(!(root->next->value == 100)) {
	
		return false;
	}
	append(root, 200);

	if(!(root->next->next->value == 200)) {
	
		return false;
	}
	if(!(root->value == 0)) {
		return false;
	}
	clear(root);
	return true;
}

bool test_prepend() {
	struct list_item *root = malloc(sizeof(struct list_item));

	prepend(root, 100);
	if(!(root->value == 100)) {
	
		return false;
	}
	prepend(root, 200);

	if(!(root->value == 200)) {
	
		return false;
	}
	if(!(root->next->next->value == 0)) {
		return false;
	}
	clear(root);
	return true;
}


bool test_input_sorted() {}





int main()
{
 	if(test_append) {
		 printf("%s", "append works");
	}

	/* Write your test cases here */
	return 0;
}
