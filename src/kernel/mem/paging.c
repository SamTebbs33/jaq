//
// Created by sam on 24/04/18.
//

#include "paging.h"
#include "mem.h"
#include "../util/string.h"
#include "../log/log.h"
#include "heap.h"
#include "../util/maths.h"

// Current system creates pages for the whole RAM space at the start
// Maybe add more pages to reflect virtual memory in swapspace.
// Detect reading from virtual memory by page fault exceptions, during which
// the kernel should swap in the associated page and continue

uint32_t num_frames;
uint32_t* frames_bitmap;

#define FRAME_OFFSET(n) (n / sizeof(uint32_t))
#define BIT_OFFSET(n) (n % sizeof(uint32_t))
#define FRAME_FROM_ADDR(a) a / PAGE_SIZE
#define ADDR_FROM_FRAME(a) a * PAGE_SIZE

#define HEAP_INDEX_SIZE 0x20000
// Address at 40MiB, should be enough
#define KERNEL_HEAP_MAX_ADDR 0x2800000

page_directory_t* kernel_directory;
heap_t* kernel_heap;
extern void* kernel_end, *kernel_start;
extern uint32_t placement_address;

// Global = maybe bad
static uint32_t addr;

void kernel_directory_entry_callback(page_dir_entry_t* entry, page_table_t* table) {
    entry->present = (uint8_t) (addr < KERNEL_HEAP_MAX_ADDR);
    entry->accessed = 0;
    // TODO: After enabling paging this may not actually be the physical address
    // Ensuring that all page tables are kept in kernel memory and
    // not moving kernel memory around should mitigate this
    entry->table_physical_addr = (uint32_t) table;
    entry->caching_disabled = 0;
    entry->global = 0;
    entry->ignored = 0;
    entry->user_level = 0;
    entry->writable = 1;
    entry->zero = 0;
    entry->write_through = 0;
    entry->four_megabyte_pages = 0;
}

void kernel_table_entry_callback(page_table_entry_t* entry) {
    bool within_address_space = addr < KERNEL_HEAP_MAX_ADDR;
    entry->accessed = 0;
    bool allocated = within_address_space ? paging_alloc_page(entry) : false;
    entry->present = (uint8_t) allocated;
    entry->write_through = 0;
    entry->zero = 0;
    entry->writable = 1;
    entry->user_level = 0;
    entry->ignored = 0;
    entry->global = 0;
    entry->caching_disabled = 0;
    entry->dirty = 0;
    addr += PAGE_SIZE;
}

void paging_init(uint32_t mem_kilobytes, uint32_t placement_addr) {
    // Align the page size to PAGE_SIZE
    // so we don't make a page table that oversteps the total memory
    uint32_t mem_kilobytes_aligned = mem_kilobytes - (mem_kilobytes % PAGE_SIZE);
    uint32_t kernel_end_addr = (uint32_t) &kernel_end;
    if(placement_addr < UINT32_MAX && placement_addr >= kernel_end_addr) {
        placement_address = placement_addr;
    } else placement_address = (uint32_t) &kernel_end;

    addr = (uint32_t) &kernel_start;

    num_frames = mem_kilobytes_aligned * 1024 / PAGE_SIZE;
    frames_bitmap = (uint32_t *) kmalloc_a(num_frames * sizeof(uint32_t));

    kernel_directory = paging_create_directory(kernel_directory_entry_callback, kernel_table_entry_callback);
    // Figure out where the kernel heap should start, based on where we have already allocated memory using placement_address
    // and what is to be alloacted using placement_address. Plus add a little extra for safety
    uint32_t heap_start = ALIGN_UP(placement_address + HEAP_OVERHEAD(HEAP_INDEX_SIZE)) + PAGE_SIZE;
    uint32_t heap_size = 0x100000;
    uint32_t heap_end = min_u32(KERNEL_HEAP_MAX_ADDR, ALIGN_UP(heap_start + heap_size));

    paging_set_directory(kernel_directory);
    // Enable paging
    uint32_t cr0;
    asm ("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x8000000;
    asm ("mov %0, %%cr0":: "r"(cr0));

    kernel_heap = heap_create(heap_start, heap_end, true, true, HEAP_INDEX_SIZE);
}

void set_frame(uint32_t frame) {
    frames_bitmap[FRAME_OFFSET(frame)] |= ((uint32_t) 1 << BIT_OFFSET(frame));
}

void clear_frame(uint32_t frame) {
    frames_bitmap[FRAME_OFFSET(frame)] &= ~((uint32_t) 1 << BIT_OFFSET(frame));
}

bool get_frame(uint32_t frame) {
    return (frames_bitmap[FRAME_OFFSET(frame)] & ((uint32_t) 1 << BIT_OFFSET(frame))) != 0;
}

uint32_t first_free_frame() {
    for (uint32_t i = 0; i < num_frames; ++i) {
        uint32_t frame = frames_bitmap[i];
        // If not full
        if(frame != 0xFFFFFFFF) {
            for (uint32_t j = 0; j < sizeof(uint32_t); ++j) {
                if ((frame & (1 << j)) == 0) return i * sizeof(uint32_t) + j;
            }
        }
    }
    return UINT32_MAX;
}

bool paging_alloc_page(page_table_entry_t *page) {
    uint32_t frame = first_free_frame();
    if(frame < UINT32_MAX) {
        page->present = 1;
        page->physical_addr = frame * PAGE_SIZE;
        page->dirty = 0;
        page->accessed = 0;
        set_frame(frame);
        return true;
    }
    return false;
}

void paging_free_page(page_table_entry_t *page) {
    uint32_t frame = page->physical_addr / PAGE_SIZE;
    if(get_frame(frame)) {
        clear_frame(frame); // TODO: Explore if freeing causes problems for other pages referencing this frame. Maybe keep reference count?
        page->present = 0;
    } else log_error("Freeing unallocated frame");
}

page_table_entry_t *paging_get_page(uint32_t addr, page_directory_t *directory) {
    uint32_t page = addr / PAGE_SIZE;
    uint32_t table_idx = page / (PAGE_ENTRIES_PER_TABLE * PAGE_TABLE_ENTRIES_PER_DIRECTORY);
    uint32_t page_idx = (page / PAGE_TABLE_ENTRIES_PER_DIRECTORY) / PAGE_ENTRIES_PER_TABLE;
    return &kernel_directory->tables[table_idx]->entries[page_idx];
}

page_dir_entry_t* paging_get_table(uint32_t addr) {
    uint32_t page = addr / PAGE_SIZE;
    uint32_t table_idx = page / (PAGE_ENTRIES_PER_TABLE * PAGE_TABLE_ENTRIES_PER_DIRECTORY);
    return &kernel_directory->entries[table_idx];
}

void paging_set_directory(page_directory_t *directory) {
    asm ("mov %0, %%cr3":: "r"((uint32_t) directory));
}

page_directory_t* paging_create_directory(page_dir_entry_callback_t dir_callback,
                                          page_table_entry_callback_t table_callback) {
    page_directory_t *dir = (page_directory_t *) kmalloc_a(sizeof(page_directory_t));
    if (dir) {
        for (int i = 0; i < PAGE_TABLE_ENTRIES_PER_DIRECTORY; ++i) {
            page_table_t* table = (page_table_t *) kmalloc_a(sizeof(page_table_t));
            page_dir_entry_t* dir_entry = &dir->entries[i];
            kernel_directory->tables[i] = table;
            dir_callback(dir_entry, table);

            for (int j = 0; j < PAGE_ENTRIES_PER_TABLE; ++j) {
                page_table_entry_t* table_entry = &table->entries[j];
                table_callback(table_entry);
            }
        }
    }
    return dir;
}
