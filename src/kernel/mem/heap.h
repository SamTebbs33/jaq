//
// Created by Samuel Tebbs on 16/04/2018.
//

#ifndef JAQ_HEAP_H
#define JAQ_HEAP_H

#include <stdinc.h>

#define HEAP_MAGIC 0x3BADDAD3

typedef struct {
    size_t size;
    uint32_t magic;
    bool is_hole;
} heap_header_t;

typedef struct {
    uint32_t magic;
    heap_header_t* header;
} heap_footer_t;

typedef struct {
    heap_header_t** entries;
    size_t size, max_size;
} heap_index_t;

typedef struct {
    uint32_t start_addr, end_addr;
    size_t occupied, capacity;
    heap_index_t* hole_index;
    bool is_kernel, is_writable;
} heap_t;

#define HEAP_OVERHEAD(num_entries) sizeof(heap_t) + sizeof(heap_index_t) + sizeof(heap_header_t*) * num_entries

heap_t* heap_create(uint32_t start_addr, uint32_t end_addr, bool is_kernel, bool is_writable, uint32_t index_size);
void* heap_alloc(size_t size, heap_t* heap);
void heap_free(void* ptr, heap_t* heap);

#endif //JAQ_HEAP_H
