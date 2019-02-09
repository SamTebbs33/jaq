//
// Created by sam on 09/02/19.
//

#include <util/linkedlist.h>
#include <mem/mem.h>

linkedlist_node_t *make_node(void *ptr, linkedlist_node_t *prev, linkedlist_node_t *next);

linkedlist_node_t* get_node(linkedlist_t *list, uint32_t i);

linkedlist_t *linkedlist_create() {
    linkedlist_t* list = kmalloc(sizeof(linkedlist_t));
    if(list) {
        list->head = list->tail = NULL;
        list->size = 0;
    }
    return list;
}

bool linkedlist_add(linkedlist_t *list, void *ptr) {
    if(list->size == INT32_MAX) return false;
    linkedlist_node_t* node = make_node(ptr, list->tail, NULL);
    if(!node) return false;
    if(list->head && list->tail) list->tail->next = node;
    else list->head = node;
    list->tail = node;
    list->size++;
    return true;
}

void *linkedlist_get(linkedlist_t *list, uint32_t i) {
    if(list->size <= i) return NULL;
    return get_node(list, i)->ptr;
}

linkedlist_node_t* get_node(linkedlist_t *list, uint32_t i) {
    linkedlist_node_t* node = list->head;
    for (uint32_t j = 0; j < i; ++j) node = node->next;
    return node;
}

size_t linkedlist_size(linkedlist_t *list) {
    return list->size;
}

bool linkedlist_remove(linkedlist_t *list, uint32_t i) {
    if(list->size <= i) return false;
    linkedlist_node_t* node = get_node(list, i);
    if(node->next) node->next->prev = node->prev;
    if(node->prev) node->prev->next = node->next;
    if(i == 0) list->head = node->next;
    if(i == list->size - 1) list->tail = node->prev;
    list->size--;
    kfree(node);
    return true;
}

linkedlist_node_t *make_node(void *ptr, linkedlist_node_t *prev, linkedlist_node_t *next) {
    linkedlist_node_t* node = kmalloc(sizeof(linkedlist_node_t));
    if(node) {
        node->next = next;
        node->prev = prev;
        node->ptr = ptr;
    }
    return node;
}

