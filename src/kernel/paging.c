//
// Created by Samuel Tebbs on 13/04/2018.
//

#include "paging.h"
#include "util.h"
#include "mem.h"
#include "heap.h"
#include "interrupts.h"

// Bitmap for frames
uint32_t* frames;
uint32_t n_frames;

extern void* kernel_end;
uint32_t placement_address = (uint32_t) &kernel_end;
page_directory_t* kernel_directory;
heap_t* kernel_heap = NULL;

#define BITS_PER_FRAME 32
#define FULL_FRAME 0xFFFFFFFF
#define FRAME_OFFSET(n) (n / (BITS_PER_FRAME))
#define BIT_OFFSET(n) (n % BITS_PER_FRAME)
#define FRAME_FROM_ADDR(a) a / BYTES_PER_PAGE
#define ADDR_FROM_FRAME(a) a * BYTES_PER_PAGE

#define KHEAP_START         0xC0000000
#define KHEAP_INITIAL_SIZE  0x100000
#define HEAP_INDEX_SIZE   0x20000
#define HEAP_MIN_SIZE     0x70000

void page_fault(interrupt_registers_t registers) ;

void set_frame(uint32_t frame_addr) {
    uint32_t frame = FRAME_FROM_ADDR(frame_addr);
    frames[FRAME_OFFSET(frame)] |= (1 << BIT_OFFSET(frame));
}

void clear_frame(uint32_t frame_addr) {
    uint32_t frame = FRAME_FROM_ADDR(frame_addr);
    frames[FRAME_OFFSET(frame)] &= ~(1 << BIT_OFFSET(frame));
}

int32_t get_frame(uint32_t frame_addr) {
    uint32_t frame = FRAME_FROM_ADDR(frame_addr);
    return (frames[FRAME_OFFSET(frame)] & (1 << BIT_OFFSET(frame))) != 0;
}

uint32_t first_free_frame() {
    for (uint32_t i = 0; i < FRAME_OFFSET(n_frames); ++i) {
        uint32_t frame = frames[i];
        // If at least one bit in the frame is free
        if(frame != FULL_FRAME) {
            for (int j = 0; j < BITS_PER_FRAME; ++j) {
                uint32_t full = (uint32_t) (0x1 << j);
                if(! (frame & full)) return (uint32_t) (i * BITS_PER_FRAME + j);
            }
        }
    }
    return UINT32_MAX;
}

void alloc_frame(page_t* page, bool is_kernel, bool is_writeable) {
    if (page->frame != 0) return;
    else {
        uint32_t idx = first_free_frame();
        if (idx == UINT32_MAX) PANIC("No free frames!");
        set_frame(ADDR_FROM_FRAME(idx));
        page->present = 1;
        page->writable = is_writeable ? 1 : 0;
        page->user_level = is_kernel ? 0 : 1;
        page->frame = idx;
    }
}

void free_frame(page_t* page) {
    if(!page->frame) return;
    clear_frame(page->frame);
    page->frame = 0;
}

void paging_init(uint32_t mem_kilobytes, uint32_t addr) {
    if(addr < UINT32_MAX) placement_address = addr;
    n_frames = FRAME_FROM_ADDR(mem_kilobytes * 1024);
    frames = (uint32_t *) kmalloc(FRAME_OFFSET(n_frames));
    memset(frames, FRAME_OFFSET(n_frames), 0);

    kernel_directory = (page_directory_t *) kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, sizeof(page_directory_t), 0);

    // Map some pages in the kernel heap area.
    // Here we call get_page but not alloc_frame. This causes page_table_t's
    // to be created where necessary. We can't allocate frames yet because they
    // they need to be identity mapped first below, and yet we can't increase
    // placement_address between identity mapping and enabling the heap!
    uint32_t i = 0;
    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += BYTES_PER_PAGE)
        paging_get_page(i, kernel_directory, true);

    // We need to identity map (phys addr = virt addr) from
    // 0x0 to the end of used memory, so we can access this
    // transparently, as if paging wasn't enabled.
    // Allocate BYTES_PER_PAGE extra so the kernel heap can be
    // initialised properly.
    i = 0;
    while (i < placement_address + BYTES_PER_PAGE)
    {
        // Kernel code is readable but not writeable from userspace.
        alloc_frame( paging_get_page(i, kernel_directory, true), false, false);
        i += BYTES_PER_PAGE;
    }

    // Now allocate those pages we mapped earlier.
    for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += BYTES_PER_PAGE)
        alloc_frame( paging_get_page(i, kernel_directory, true), false, false);

    interrupts_register_handler(ISR_14, page_fault);
    paging_set_directory(kernel_directory);

    // Enable paging
    uint32_t cr0;
    asm ("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;
    asm ("mov %0, %%cr0":: "r"(cr0));

    kernel_heap = heap_create(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, false, false, HEAP_INDEX_SIZE);
}

void paging_set_directory(page_directory_t *new) {
    asm ("mov %0, %%cr3":: "r"(&new->tables_physical));
}

page_t *paging_get_page(uint32_t addr, page_directory_t *page_dir, bool create) {
    addr = FRAME_FROM_ADDR(addr);
    uint32_t table_idx = addr / PAGES_PER_TABLE;
    uint32_t page_idx = addr % PAGES_PER_TABLE;
    if(page_dir->tables[table_idx]) return &page_dir->tables[table_idx]->pages[page_idx];
    else if(create) {
        uint32_t tmp;
        page_table_t* table = (page_table_t *) kmalloc_ap(sizeof(page_table_t), &tmp);
        memset(table, BYTES_PER_PAGE, 0);
        page_dir->tables_physical[table_idx] = tmp | 0x7; //Present, writable and user level
        page_dir->tables[table_idx] = table;
        return &table->pages[page_idx];
    }
    return NULL;
}

void page_fault(interrupt_registers_t registers) {
    PANIC("Page fault");
}
