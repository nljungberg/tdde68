#include "list.h"
#include<stdio.h>
#include <stdlib.h>

void append(struct list_item *first, int x)
{
 
    if (first == NULL) {
        return;
    }

    struct list_item *new_item = malloc(sizeof(struct list_item));
    new_item->value = x;
    new_item->next = NULL;


    struct list_item *current = first;
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = new_item;
}

void prepend(struct list_item *first, int x) {
    if (first == NULL) {
        return;
    }

    struct list_item *new_item = malloc(sizeof(struct list_item));
    new_item->value = first->value;
    new_item->next = first->next;

    first->value = x;
    first->next = new_item;
}


/* Find the first element in the list larger than x and
 * put x right before that element */

void input_sorted(struct list_item *first, int x) { 

    struct list_item *new_item = malloc(sizeof(struct list_item));
    new_item->value = x;
    new_item->next = NULL;
    struct list_item *current = first;
     if (first == NULL || first->value > x) {
        prepend(first,x);
        return;
     }
    while(current != NULL|| current->value > x){
        if(current->next == NULL){
            append(current, x);
            return;}
        current = current->next;
    }
    prepend(current, x);
}

void print(struct list_item *first){
     struct list_item *current;
     current = first;
    do{
        printf("%i\n", current->value);
        current = current->next;
    }
    while(current != NULL);
}

void clear(struct list_item* first){
    struct list_item *current = first;
    while (current != NULL) {
        struct list_item *temp = current;
        current = current->next;
        free(temp);
    }
}
