//
// Created by Samuel Tebbs on 16/04/2018.
//

#include "heap.h"
#include "mem.h"
#include "paging.h"
#include "util.h"

bool is_page_aligned(uint32_t addr);

void split_right(heap_header_t *hole, heap_t *heap);

void remove_hole(heap_header_t *hole, heap_index_t *index);

bool join_right(heap_header_t *header, heap_t *heap);

bool join_left(heap_header_t *header, heap_t *heap);

void add_hole(heap_header_t *hole, heap_index_t *index);

heap_index_t* heap_index_create(size_t max_size) {
    heap_index_t* index = (heap_index_t *) kmalloc(sizeof(heap_index_t));
    if(index) {
        index->size = 0;
        index->max_size = max_size;
        index->entries = (heap_header_t **) kmalloc(max_size * sizeof(heap_header_t*));
    }
    return index;
}

heap_t *heap_create(uint32_t start_addr, uint32_t end_addr, bool is_kernel, bool is_writable, uint32_t index_size) {
    if(start_addr < end_addr && is_page_aligned(start_addr) && is_page_aligned(end_addr)) {
        heap_t *heap = (heap_t *) kmalloc(sizeof(heap_t));
        if(heap) {
            heap->start_addr = start_addr;
            heap->end_addr = end_addr;
            heap->is_kernel = is_kernel;
            heap->is_writable = is_writable;
            heap->capacity = end_addr - start_addr;
            heap->occupied = 0;
            heap->hole_index = heap_index_create(index_size);
        }
        return heap;
    }
    return NULL;
}

heap_header_t* find_hole(size_t size, heap_t* heap) {
    heap_index_t* index = heap->hole_index;
    size_t best_size = SIZE_MAX;
    heap_header_t* best_header = NULL;
    for (int i = 0; i < index->size; ++i) {
        heap_header_t* header = index->entries[i];
        if(header->is_hole && header->size >= size) {
            if(header->size == size) {
                best_header = header;
                best_size = size;
                break;
            } else if(header->size < best_size) {
                best_header = header;
                best_size = header->size;
            }
        }
    }
    return best_header;
}

void *heap_alloc(size_t size, heap_t *heap) {
    if(heap->capacity - heap->occupied >= size) {
        heap_header_t* hole = find_hole(size, heap);
        if(hole) {
            size_t overhead = sizeof(heap_header_t) + sizeof(heap_footer_t);
            // Memory in the hole that exceeds what was requested
            size_t surplus = hole->size - size;
            remove_hole(hole, heap->hole_index);
            if(overhead < surplus) {
                // If the overhead of adding another footer and header is worth the extra hole gained, then split the hole in two
                split_right(hole, heap);
            }
            hole->is_hole = FALSE;
            uint32_t data_addr = (uint32_t) hole + sizeof(heap_header_t);
            return (void *) data_addr;
        }
    }
    return NULL;
}

void heap_free(void *ptr, heap_t *heap) {
    uint32_t ptr_addr = (uint32_t) ptr;
    if(ptr && ptr_addr < heap->end_addr && ptr_addr > heap->start_addr) {
        heap_header_t* header = (heap_header_t *) (ptr_addr - sizeof(heap_header_t));
        if(header && header->magic == HEAP_MAGIC) {
            header->is_hole = TRUE;
            join_right(header, heap);
            // If this hole wasn't incorporated into the left one, then add it to the hole index
            if(!join_left(header, heap)) add_hole(header, heap);
            return;
        }
    }
    PANIC("Freeing unallocated memory");
}

void add_hole(heap_header_t *hole, heap_index_t *index) {
    if(index->size < index->max_size) index->entries[index->size++] = hole;
}

bool join_left(heap_header_t *header, heap_t *heap) {
    // TODO
}

bool join_right(heap_header_t *header, heap_t *heap) {
    // TODO
}

void remove_hole(heap_header_t *hole, heap_index_t *index) {
    uint32_t i = 0;
    // Find hole after this one
    while(i < index->size && index->entries[i++] != hole);
    if(i > 0) {
        // Shift subsequent entries left, the last one isn't replace but that's fine
        while (i > 0 && i < index->size) index->entries[i - 1] = index->entries[i++];
        index->size--;
    }
}

void split_right(heap_header_t *hole, heap_t *heap) {
    // TODO
}

bool is_page_aligned(uint32_t addr) {
    return (bool) ((addr & BYTES_PER_PAGE > 0) ? TRUE : FALSE);
}