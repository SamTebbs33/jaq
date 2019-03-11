//
// Created by Samuel Tebbs on 16/04/2018.
//

#include <mem/heap.h>
#include <mem/mem.h>
#include <lib/util.h>
#include <arch.h>

bool is_page_aligned(uint32_t addr);

void split_right(heap_header_t *hole, size_t size, heap_t *heap);

void remove_hole(heap_header_t *hole, heap_index_t *index);

bool join_right(heap_header_t *header, heap_t *heap);

bool join_left(heap_header_t *header, heap_t *heap);

void add_hole(heap_header_t *hole, heap_index_t *index);

heap_index_t* heap_index_create(size_t max_size) {
    heap_index_t* index = (heap_index_t *) kmalloc(sizeof(heap_index_t));
    ASSERT_NOT_NULL("heap index allocation", index);
    if(index) {
        index->size = 0;
        index->max_size = max_size;
        index->entries = (heap_header_t **) kmalloc(max_size * sizeof(heap_header_t*));
        ASSERT_NOT_NULL("heap entries allocation", index->entries);
    }
    return index;
}

heap_t *heap_create(uint32_t start_addr, uint32_t end_addr, bool is_kernel, bool is_writable, uint32_t index_size) {
    heap_t *heap = (heap_t *) kmalloc(sizeof(heap_t));
    ASSERT_NOT_NULL("heap allocation", heap);
    if(heap) {
        heap->start_addr = start_addr;
        heap->end_addr = end_addr;
        heap->is_kernel = is_kernel;
        heap->is_writable = is_writable;
        heap->capacity = end_addr - start_addr;
        heap->occupied = 0;
        heap->hole_index = heap_index_create(index_size);

        heap_header_t* initial_hole = (heap_header_t *) start_addr;
        initial_hole->magic = HEAP_MAGIC;
        initial_hole->is_hole = true;
        initial_hole->size = end_addr - start_addr - sizeof(heap_header_t) - sizeof(heap_footer_t);
        add_hole(initial_hole, heap->hole_index);
    }
    return heap;
}

heap_header_t* find_hole(size_t size, heap_t* heap) {
    heap_index_t* index = heap->hole_index;
    size_t best_size = SIZE_MAX;
    heap_header_t* best_header = NULL;
    for (uint32_t i = 0; i < index->size; ++i) {
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
                split_right(hole, surplus - overhead, heap);
            }
            hole->is_hole = false;
            uint32_t data_addr = (uint32_t) hole + sizeof(heap_header_t);
            // The memory taken up by the block is now occupied
            heap->occupied += hole->size + sizeof(heap_header_t) + sizeof(heap_footer_t);
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
            header->is_hole = true;
            //join_right(header, heap);
            // If this hole wasn't incorporated into the left one, then add it to the hole index
            //if(!join_left(header, heap)) add_hole(header, heap->hole_index);
            // The memory taken up by the block is released
            heap->occupied -= header->size + sizeof(heap_header_t) + sizeof(heap_footer_t);
        }
    } else PANIC("Freeing unallocated memory");
}

void add_hole(heap_header_t *hole, heap_index_t *index) {
    if(index->size < index->max_size) index->entries[index->size++] = hole;
}

bool join_left(heap_header_t *header, heap_t *heap) {
    uint32_t left_addr = (uint32_t) header - sizeof(heap_footer_t);
    if(left_addr > heap->start_addr) {
        heap_footer_t* left_footer = (heap_footer_t *) left_addr;
        if(left_footer->magic == HEAP_MAGIC) {
            heap_header_t* left_header = left_footer->header;
            if(left_header->is_hole) {
                heap_footer_t* footer = (heap_footer_t *) ((uint32_t) header + sizeof(heap_header_t) + header->size);

                footer->header = left_header;
                left_header->size += sizeof(header) + sizeof(footer) + header->size;

                header->magic = 0;
                left_footer->magic = 0;

                remove_hole(header, heap->hole_index);
                join_left(left_header, heap); // Recurse until we find a non-hole or memory outside of the heapspace
                return true;
            }
        }
    }
    return false;
}

bool join_right(heap_header_t *header, heap_t *heap) {
    if(header && header->is_hole) {
        uint32_t right_addr = (uint32_t) header + sizeof(header) + header->size + sizeof(heap_footer_t);
        // Ensure the right address is within heapspace
        if(right_addr <= heap->end_addr) {
            heap_header_t* right_header = (heap_header_t *) right_addr;
            if(right_header->magic == HEAP_MAGIC && right_header->is_hole) {
                size_t right_size = right_header->size;

                // Find the right footer and left footer
                heap_footer_t* right_footer = (heap_footer_t *) (right_addr + sizeof(header) + right_size);
                heap_footer_t* footer = (heap_footer_t *) (right_addr - sizeof(heap_footer_t));

                // Make right footer point to left header (cuts out right header)
                right_footer->header = header;
                footer->magic = 0;
                right_header->magic = 0;
                header->size += sizeof(heap_footer_t) + sizeof(heap_header_t) + right_size;

                remove_hole(right_header, heap->hole_index);
                join_right(header, heap); // Recurse until we find a non-hole or memory outside of the heapspace
                return true;
            }
        }
    }
    return false;
}

void remove_hole(heap_header_t *hole, heap_index_t *index) {
    uint32_t i = 0;
    // Find hole after this one
    while(i < index->size && index->entries[i++] != hole);
    if(i > 0) {
        // Shift subsequent entries left, the last one isn't replace but that's fine
        while (i > 0 && i < index->size) {
            index->entries[i - 1] = index->entries[i];
            i++;
        }
        index->size--;
    }
}

void split_right(heap_header_t *hole, size_t size, heap_t *heap) {
    // Check if the heap index can fit more hole entries
    if(hole && hole->is_hole && heap->hole_index->size < heap->hole_index->max_size) {
        // hole -> (hole->size) -> footer
        size_t left_hole_size = hole->size - size - sizeof(heap_footer_t) - sizeof(heap_header_t);
        size_t right_hole_size = size;

        // Find addresses of new footers and header
        heap_footer_t *left_footer = (heap_footer_t *) ((uint32_t) hole + sizeof(heap_header_t) + left_hole_size);
        heap_header_t *right_header = (heap_header_t *) ((uint32_t) left_footer + sizeof(heap_footer_t));
        heap_footer_t *right_footer = (heap_footer_t *) ((uint32_t) right_header + sizeof(heap_header_t) + right_hole_size);
        // hole -> (left_hole_size) -> left_footer, right_header -> (right_hole_size) -> right_footer

        hole->size = left_hole_size;

        left_footer->magic = HEAP_MAGIC;
        left_footer->header = hole;

        right_header->magic = HEAP_MAGIC;
        right_header->size = right_hole_size;
        right_header->is_hole = true;

        right_footer->magic = HEAP_MAGIC;
        right_footer->header = right_header;

        add_hole(right_header, heap->hole_index);
    }
}

bool is_page_aligned(uint32_t addr) {
    return ((addr & ARCH_PAGE_SIZE) > 0) ? true : false;
}