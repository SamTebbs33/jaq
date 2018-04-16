//
// Created by Samuel Tebbs on 13/04/2018.
//

#include "mem.h"
#include "paging.h"
#include "heap.h"

uint32_t placement_address;
extern heap_t* kernel_heap;
extern page_directory_t* kernel_directory;

uint32_t _kmalloc(size_t size, bool align, uint32_t *phys) {
    if(kernel_heap != NULL) {
        void* addr = heap_alloc(size, kernel_heap);
        if(phys) {
            page_t* page = paging_get_page((uint32_t) addr, kernel_directory, FALSE);
            *phys = page->frame * BYTES_PER_PAGE + ((uint32_t) addr & 0xFFF);
        }
        return (uint32_t) addr;
    } else {
        // If the address is not already page-aligned
        if (align && (placement_address & 0xFFFFF000)) {
            // Align it
            placement_address &= 0xFFFFF000;
            placement_address += 0x1000;
        }
        if (phys) {
            *phys = placement_address;
        }
        uint32_t tmp = placement_address;
        placement_address += size;
        return tmp;
    }
}

uint32_t kmalloc(size_t size) {
    return _kmalloc(size, FALSE, NULL);
}

uint32_t kmalloc_a(size_t size) {
    return _kmalloc(size, TRUE, NULL);
}

uint32_t kmalloc_p(size_t size, uint32_t *phys) {
    return _kmalloc(size, FALSE, phys);
}

uint32_t kmalloc_ap(size_t size, uint32_t *phys) {
    return _kmalloc(size, TRUE, phys);
}

void kfree(void *ptr) {
    heap_free(ptr, kernel_heap);
}
