//
// Created by sam on 09/02/19.
//

#ifndef JAQ_LINKEDLIST_H
#define JAQ_LINKEDLIST_H

#include <stdinc.h>

typedef struct linkedlist_node {
    void* ptr;
    struct linkedlist_node* next, *prev;
} linkedlist_node_t;

typedef struct linkedlist {
    linkedlist_node_t* head, *tail;
    size_t size;
} linkedlist_t;

linkedlist_t* linkedlist_create();

bool linkedlist_add(linkedlist_t *list, void *ptr);

void* linkedlist_get(linkedlist_t* list, uint32_t i);

size_t linkedlist_size(linkedlist_t* list);

bool linkedlist_remove(linkedlist_t *list, uint32_t i);

#endif //JAQ_LINKEDLIST_H
