#include "list.h"
#include <stdio.h>
#include <stdlib.h>

void append(struct list_item *first, int x)
{
    if (first == NULL) {
        return; 
    }

    struct list_item* new_item = malloc(sizeof(struct list_item));
    new_item->value = x;
    new_item->next = NULL;

    struct list_item *current = first;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = new_item;
}

void prepend(struct list_item *first, int x)
{
    if (first == NULL) {
        return; 
    }

    struct list_item *new_item = malloc(sizeof(struct list_item));
    new_item->value = first->value;
    new_item->next = first->next;

    first->value = x;

    first->next = new_item;
}

void input_sorted(struct list_item *first, int x)
{
    if (first == NULL) {
        return; 
    }


    if (x < first->value) {
        prepend(first, x);
        return;
    }

    struct list_item *current = first;
    while (current->next != NULL && current->next->value <= x) {
        current = current->next;
    }

    struct list_item *new_item = malloc(sizeof(struct list_item));
    new_item->value = x;
    new_item->next = current->next;

    current->next = new_item;
}

void print(struct list_item *first)
{
    struct list_item *current = first;
    while (current != NULL) {
        printf("%d\n", current->value);
        current = current->next;
    }
}

void clear(struct list_item *first)
{
    struct list_item *current = first;
    first = first->next;
    current->next = NULL;
    while (first != NULL) {
        current = first;
        first = first->next;
        free(current);
    }
}
