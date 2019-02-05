//
// Created by Samuel Tebbs on 13/04/2018.
//

#ifndef JAQ_MEM_H
#define JAQ_MEM_H

#include <stdinc.h>

#define KMALLOC_N(type, n) (type*) kmalloc(sizeof(type) * n)
#define KMALLOC(type) KMALLOC_N(type, 1)

#define KMALLOC_N(type, n) (type*) kmalloc(sizeof(type) * n)
#define KMALLOC(type) KMALLOC_N(type, 1)

/**
 * Allocate kernel memory on the kernel heap, or the kernel pile if a heap isn't set up
 * @param size The size of memory to allocate
 * @return The pointer to the memory allocated or NULL if it failed
 */
void* kmalloc(size_t size); // Normal

/**
 * Allocate kernel memory to a page-aligned address on the kernel heap, or the kernel pile if a heap isn't set up
 * @param size The size of memory to allocate
 * @return The pointer to the memory allocated or NULL if it failed
 */
void* kmalloc_a(size_t size); // Page-aligned

/**
 * Free some kernel memory. Should have been allocated with kmalloc or kmalloc_a.
 * Allocating before setting up the heap and freeing after is undefined behaviour
 * @param ptr The pointer to the memory to free
 */
void kfree(void* ptr);

#endif //JAQ_MEM_H
