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


bool test_input_sorted() {
	struct list_item *root = malloc(sizeof(struct list_item));
	input_sorted(root, 100);
	if(!(root->next->value == 100)){
		return false;
	}

	input_sorted(root, 50);
	if(!(root->next->value == 50)){
		return false;
	}

	input_sorted(root, 75);
	if(!(root->next->next->value == 75)){
		return false;
	}

	clear(root);
	return true;
}

bool test_clear(){
	struct list_item *root = malloc(sizeof(struct list_item));
	append(root, 30);
	prepend(root, 5);
	prepend(root, 10);
	clear(root);
	if(!(root->next == NULL)){
		return false;
	}
	return true;
}





int main()
{
 	if(test_append) {
		 printf("%s\n", "append works");
	}
	if(test_prepend){
		printf("%s\n", "prepend works");
	}
	if(test_input_sorted){
		printf("%s\n", "input_sorted works");
	}
	if(test_clear){
		printf("%s\n", "clear works");
	}
	/* Write your test cases here */
	return 0;
}
