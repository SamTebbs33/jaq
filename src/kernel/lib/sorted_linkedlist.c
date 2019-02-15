//
// Created by sam on 14/02/19.
//

#include <lib/sorted_linkedlist.h>

bool sorted_linkedlist_add(linkedlist_t *list, void *ptr, comparator_t cmp) {
    uint32_t i = 0;
    while (i < linkedlist_size(list) && cmp(ptr, linkedlist_get(list, i)) > 0) i++;
    return linkedlist_insert(list, ptr, i);
}

