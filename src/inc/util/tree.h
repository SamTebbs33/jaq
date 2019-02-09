//
// Created by sam on 09/02/19.
//

#ifndef JAQ_TREE_H
#define JAQ_TREE_H

#include "linkedlist.h"

typedef struct tree {
    void* ptr;
    linkedlist_t* children;
} tree_t;

tree_t *tree_create(void *ptr);

bool tree_add(tree_t *tree, void *ptr);

bool tree_remove(tree_t* tree, uint32_t child);

void* tree_get(tree_t* tree, uint32_t child);

#endif //JAQ_TREE_H
