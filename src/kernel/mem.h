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
uint32_t kmalloc_p(size_t size, uint32_t* phys); // Returns physical address
uint32_t kmalloc_ap(size_t size, uint32_t* phys); // Page-aligned and returns physical address
void kfree(void* ptr);

#endif //JAQ_MEM_H
