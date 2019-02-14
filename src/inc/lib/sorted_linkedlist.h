//
// Created by sam on 14/02/19.
//

#ifndef JAQ_SORTED_LINKEDLIST_H
#define JAQ_SORTED_LINKEDLIST_H

#include <stdinc.h>
#include <lib/linkedlist.h>

typedef int (*comparator_t)(void* ptr1, void* ptr2);

bool sorted_linkedlist_add(linkedlist_t* list, void* ptr, comparator_t cmp);

#endif //JAQ_SORTED_LINKEDLIST_H
