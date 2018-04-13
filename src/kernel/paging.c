//
// Created by Samuel Tebbs on 13/04/2018.
//

#include "paging.h"
#include "util.h"
#include "mem.h"
#include "interrupts.h"

// Bitmap for frames
uint32_t* frames;
uint32_t n_frames;

extern void* kernel_end;
uint32_t placement_address = (uint32_t) &kernel_end;

#define BITS_PER_FRAME 32
#define FULL_FRAME 0xFFFFFFFF
#define FRAME_OFFSET(n) (n / (BITS_PER_FRAME))
#define BIT_OFFSET(n) (n % BITS_PER_FRAME)
#define FRAME_FROM_ADDR(a) a / 0x1000
#define ADDR_FROM_FRAME(a) a * 0x1000

page_directory_t* kernel_directory;

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

void paging_init(uint32_t mem_size) {
    n_frames = ADDR_FROM_FRAME(mem_size);
    frames = (uint32_t *) kmalloc(FRAME_OFFSET(n_frames));
    memset(frames, FRAME_OFFSET(n_frames), 0);

    kernel_directory = (page_directory_t *) kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, sizeof(page_directory_t), 0);

    for (uint32_t i = 0; i < placement_address; i += 0x1000) {
        // Allocate a frame for kernel that is non-writable from user space
        alloc_frame(paging_get_page(i, kernel_directory, TRUE), FALSE, FALSE);
    }

    interrupts_register_handler(IRQ_14, page_fault);
    paging_set_directory(kernel_directory);
}

void paging_set_directory(page_directory_t *new) {
    asm volatile("mov %0, %%cr3":: "r"(&new->tables_physical));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging and put new value back into cr0
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *paging_get_page(uint32_t addr, page_directory_t *page_dir, bool create) {
    addr = FRAME_FROM_ADDR(addr);
    uint32_t table_idx = addr / PAGES_PER_TABLE;
    uint32_t page_idx = addr % PAGES_PER_TABLE;
    if(page_dir->tables[table_idx]) return &page_dir->tables[table_idx]->pages[page_idx];
    else if(create) {
        uint32_t tmp;
        page_table_t* table = (page_table_t *) kmalloc_ap(sizeof(page_table_t), &tmp);
        memset(table, 0x1000, 0);
        page_dir->tables_physical[table_idx] = tmp | 0x7; //Present, writable and user level
        page_dir->tables[table_idx] = table;
        return &table->pages[page_idx];
    }
    return NULL;
}

void page_fault(interrupt_registers_t registers) {
    PANIC("Page fault");
}
