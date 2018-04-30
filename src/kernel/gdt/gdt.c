//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "gdt.h"

gdt_entry_t gdt[GDT_NUM_ENTRIES];
gdt_pointer_t gdt_ptr;

extern void gdt_flush(uint32_t gdt_ptr_addr);
extern void tss_flush();

tss_t tss;

void gdt_init(uint32_t kernel_stack_vaddr, uint16_t segment_selector) {
    gdt_ptr.base = (uint32_t) &gdt;
    gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_NUM_ENTRIES) - 1;

    gdt_set_entry(0, 0, 0, 0, 0);                // Null segment
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

    tss.esp0 = kernel_stack_vaddr;
    tss.ss0 = segment_selector;
    gdt_set_tss_entry(5, (uint32_t) &tss);

    gdt_flush((uint32_t) &gdt_ptr);
    tss_flush();
}

void gdt_set_entry(uint32_t idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt[idx].base_low    = (uint16_t) (base & 0xFFFF);
    gdt[idx].base_middle = (uint8_t) ((base >> 16) & 0xFF);
    gdt[idx].base_high   = (uint8_t) ((base >> 24) & 0xFF);

    gdt[idx].limit_low   = (uint16_t) (limit & 0xFFFF);
    gdt[idx].granularity = (uint8_t) ((limit >> 16) & 0x0F);

    gdt[idx].granularity |= granularity & 0xF0;
    gdt[idx].access      = access;
}

void gdt_set_tss_entry(uint32_t idx, uint32_t tss_vaddr) {
    gdt[idx].base_low    = (uint16_t) (tss_vaddr & 0xFFFF);
    gdt[idx].base_middle = (uint8_t) ((tss_vaddr >> 16) & 0xFF);
    gdt[idx].base_high   = (uint8_t) ((tss_vaddr >> 24) & 0xFF);

    gdt[idx].limit_low   = sizeof(tss_t) - 1;
    gdt[idx].granularity = 0;

    gdt[idx].access      = (0x01 << 7) | (0x01 << 3) | (0x01);
}

