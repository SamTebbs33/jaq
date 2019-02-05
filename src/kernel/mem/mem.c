//
// Created by Samuel Tebbs on 13/04/2018.
//

#include <mem/mem.h>
#include <paging.h>
#include <mem/heap.h>

uint32_t pile_start, pile_end, pile_ptr;
extern heap_t* kernel_heap;

void* _kmalloc(size_t size, bool align) {
    if(kernel_heap != NULL) return heap_alloc(size, kernel_heap);
    // If the address is not already page-aligned
    if (align && !IS_PAGE_ALIGNED(pile_ptr)) {
        // Align it
        pile_ptr = ALIGN_UP(pile_ptr);
    }
    if(pile_ptr >= pile_end || pile_ptr + size >= pile_end) return NULL;
    uint32_t tmp = pile_ptr;
    pile_ptr += size;
    return (void*)tmp;
}

void* kmalloc(size_t size) {
    return _kmalloc(size, false);
}

void* kmalloc_a(size_t size) {
    return _kmalloc(size, true);
}

void kfree(void *ptr) {
    heap_free(ptr, kernel_heap);
}
