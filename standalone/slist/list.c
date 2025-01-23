#include "list.h"
#include <stdlib.h>

void append(struct list_item *first, int x)
{
    struct list_item *new_item = malloc(sizeof(struct list_item));
    new_item->value = x;
    new_item->next = NULL;
    
    if (first == NULL) {
        first = new_item;
        return;
    }

    struct list_item *current = first;
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = new_item;
}

/* Puts value 'x' at the beginning of the list */
void prepend(struct list_item *first, int x) {
    struct list_item *new_item = malloc(sizeof(struct list_item));
    new_item->value = x;
    new_item->next = first;
    first = new_item;
}


/* Find the first element in the list larger than x and
 * put x right before that element */
void input_sorted(struct list_item *first, int x) { 

    struct list_item *new_item = malloc(sizeof(struct list_item));
    new_item->value = x;
    struct item_list current = first;
    struct item_list prev = NULL;
     if (first == NULL || first->value > x) {
        new_item->next = *first;
        first = new_item;
        return;
     }

    while(current != NULL){
        if(current->value > x){
            new_item->next = current;
            prev->next = next_item; 
            return;
        }
        prev = current;
        current = current->next;
    }
    prev->next = new_item;
    new_item->next = NULL
}

void print(struct list_item *first){
     struct list_item current = malloc(sizeof(struct list_item));
     current = first;
    do{
        printf(%i %s,current->value, "\n")
        current = current->next;
    }
    while(current != NULL)
}

void clear(struct list_item* first){
    struct list_item* current = first;
    do{
        current = first->next;
        free(first);
        first = current;
    }
    while(first != NULL)
}