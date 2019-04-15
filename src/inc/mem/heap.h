//
// Created by Samuel Tebbs on 16/04/2018.
//

#ifndef JAQ_HEAP_H
#define JAQ_HEAP_H

#include <stdinc.h>

#define HEAP_MAGIC 0x3BADDAD3

/**
 * A header associated with a heap entry
 */
typedef struct {
    size_t size;
    uint32_t magic;
    bool is_hole;
} heap_header_t;

/**
 * A footer associated with a heap entry
 */
typedef struct {
    uint32_t magic;
    heap_header_t* header;
} heap_footer_t;

/**
 * A heap index entry that corresponds to some headers in memory
 */
typedef struct {
    heap_header_t** entries;
    size_t size, max_size;
} heap_index_t;

/**
 * The parent heap structure
 */
typedef struct {
    uint32_t start_addr, end_addr;
    size_t occupied, capacity;
    heap_index_t* hole_index;
    bool is_kernel, is_writable;
} heap_t;

#define HEAP_OVERHEAD(num_entries) (sizeof(heap_t) + sizeof(heap_index_t) + sizeof(heap_header_t*)) * num_entries

/**
 * Create a new heap structure between a range of addresses. Make sure these addresses don't overlap
 * with another heap
 * @param start_addr The heap's start address
 * @param end_addr  The heap's end address
 * @param is_kernel If the heap is managing kernel memory or user memory
 * @param is_writable If the contents of the heap is writable or not
 * @param index_size The permissable size of the index. The bigger the index the more granular the heap can be.
 * @return The heap or NULL if creation failed
 */
heap_t* heap_create(uint32_t start_addr, uint32_t end_addr, bool is_kernel, bool is_writable, uint32_t index_size);

/**
 * Allocate some memory on the heap
 * @param size The size of the memory
 * @param heap The heap in which to allocate
 * @param alignment The desired alignment of the returned pointer. 0 for no alignment
 * @return A pointer to the allocated memory or NULL if allocation failed
 */
void* heap_alloc(size_t size, heap_t* heap, uint32_t alignment);

/**
 * Free some memory in the heap so that it can be reallocated
 * @param ptr The pointer to the memory to free. Should have come from heap_alloc on the same heap
 * @param heap The heap to free the memory in. Should be the same heap that it was allocated in
 */
void heap_free(void* ptr, heap_t* heap);

#endif //JAQ_HEAP_H
