//
// Created by sam on 10/02/19.
//

#ifndef JAQ_QUEUE_H
#define JAQ_QUEUE_H

#include "linkedlist.h"

typedef linkedlist_t queue_t;

/**
 * Create a new empty queue in memory
 * @return The new queue, or null if it couldn't be allocated
 */
queue_t* queue_create();

/**
 * Add an item to the end of the queue
 * @param queue The queue to add it to
 * @param ptr The item to add
 * @return True if it succeeded, else false
 */
bool queue_enqueue(queue_t *queue, void *ptr);

/**
 * Get and remove the item at the top of the queue
 * @param queue The queue to remove the item from
 * @return The item, can be NULL
 */
void *queue_dequeue(queue_t *queue);

/**
 * Get the size of the queue
 * @param queue The queue
 * @return The size
 */
size_t queue_size(queue_t* queue);

#endif //JAQ_QUEUE_H
