//
// Created by Samuel Tebbs on 13/04/2018.
//

#include "mem.h"
#include "paging.h"
#include "heap.h"

uint32_t placement_address;
extern heap_t* kernel_heap;
extern page_directory_t* kernel_directory;

uint32_t _kmalloc(size_t size, bool align) {
    if(kernel_heap != NULL) {
        void* addr = heap_alloc(size, kernel_heap);
        return (uint32_t) addr;
    } else {
        // If the address is not already page-aligned
        if (align && (placement_address & 0xFFFFF000)) {
            // Align it
            placement_address &= 0xFFFFF000;
            placement_address += 0x1000;
        }
        uint32_t tmp = placement_address;
        placement_address += size;
        return tmp;
    }
}

uint32_t kmalloc(size_t size) {
    return _kmalloc(size, false);
}

uint32_t kmalloc_a(size_t size) {
    return _kmalloc(size, true);
}

void kfree(void *ptr) {
    heap_free(ptr, kernel_heap);
}
