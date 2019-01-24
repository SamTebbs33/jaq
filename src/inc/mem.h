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

uint32_t kmalloc(size_t size); // Normal
uint32_t kmalloc_a(size_t size); // Page-aligned
void kfree(void* ptr);

#endif //JAQ_MEM_H
