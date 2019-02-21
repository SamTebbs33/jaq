//
// Created by Samuel Tebbs on 12/04/2018.
//

#include <gdt.h>
#include <lib/util.h>

gdt_entry_t gdt[GDT_NUM_ENTRIES];
gdt_pointer_t gdt_ptr;

extern void gdt_flush(uint32_t gdt_ptr_addr);
extern void tss_flush();

tss_t tss;

void gdt_init(uint32_t kernel_stack_vaddr, uint32_t kernel_stack_size, uint16_t segment_selector) {
    gdt_ptr.base = (uint32_t) &gdt;
    gdt_ptr.limit = sizeof(gdt) - 1;

    // Null segment
    gdt_set_entry(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    // Code segment
    gdt_set_entry(1, 0, 0xFFFFFFFF, GDT_CODE_READABLE, GDT_CONF_THIS_LEVEL, 1, GDT_TYPE_CODE, GDT_LEVEL_KERNEL, GDT_SIZE_32BIT, GDT_GRANULARITY_4KB, 1, 0, 1);
    // Data segment
    gdt_set_entry(2, 0, 0xFFFFFFFF, GDT_DATA_WRITABLE, GDT_DIR_UP, 0, GDT_TYPE_DATA, GDT_LEVEL_KERNEL, GDT_SIZE_32BIT, GDT_GRANULARITY_4KB, 1, 0, 1);
    // User code segment
    gdt_set_entry(3, 0, 0xFFFFFFFF, GDT_CODE_READABLE, GDT_CONF_THIS_LEVEL, 1, GDT_TYPE_CODE, GDT_LEVEL_USER, GDT_SIZE_32BIT, GDT_GRANULARITY_4KB, 1, 0, 1);
    // User data segment
    gdt_set_entry(4, 0, 0xFFFFFFFF, GDT_DATA_WRITABLE, GDT_DIR_UP, 0, GDT_TYPE_DATA, GDT_LEVEL_USER, GDT_SIZE_32BIT, GDT_GRANULARITY_4KB, 1, 0, 1);
    // TSS
    gdt_set_entry(5, (uint32_t)&tss, (uint32_t)&tss + sizeof(tss), GDT_TSS_READWRITE, GDT_TSS_DIRCONF, 1, GDT_TYPE_TSS, GDT_LEVEL_USER, GDT_SIZE_16BIT, GDT_GRANULARITY_BYTE, 1, 1, 0);

    // Set the TSS to initially point to kernel stack and segment
    memset(&tss, 0, sizeof(tss));
    tss.esp0 = kernel_stack_vaddr + kernel_stack_size;
    tss.ss0 = 0x10;
    tss.iomap_base = sizeof(tss);
    tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x10;
    tss.cs = 0x8;

    gdt_flush((uint32_t) &gdt_ptr);
    tss_flush();
}

void gdt_set_entry(uint32_t idx, uint32_t base, uint32_t limit, uint8_t read_write, uint8_t dir_conf, uint8_t code,
                   uint8_t type, uint8_t level, uint8_t size, uint8_t granularity, uint8_t present, uint8_t accessed, uint8_t available) {
    gdt_entry_t* entry = &gdt[idx];
    entry->limit_low = (uint16_t) (limit & 0xFFFF);
    entry->base_low = base & 0xFFFF;
    entry->accessed = accessed;
    entry->read_write = read_write;
    entry->dir_conf = dir_conf;
    entry->code = code;
    entry->type = type;
    entry->level = level;
    entry->present = present;
    entry->limit_high = (uint8_t) ((limit >> 16) & 0xF);
    entry->always_0 = 0;
    entry->size = size;
    entry->granularity = granularity;
    entry->base_high = (base >> 24) & 0xFFFF;
}

