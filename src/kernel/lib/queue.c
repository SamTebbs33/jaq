//
// Created by sam on 10/02/19.
//

#include <lib/queue.h>

queue_t *queue_create() {
    return linkedlist_create();
}

bool queue_enqueue(queue_t *queue, void *ptr) {
    return linkedlist_add(queue, ptr);
}

void *queue_dequeue(queue_t *queue) {
    if(linkedlist_size(queue) >= 1) {
        void *item = linkedlist_get(queue, 0);
        linkedlist_remove(queue, 0);
        return item;
    }
    return NULL;
}

size_t queue_size(queue_t *queue) {
    return linkedlist_size(queue);
}

