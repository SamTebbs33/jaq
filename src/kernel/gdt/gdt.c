//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "gdt.h"

gdt_entry_t gdt[GDT_NUM_ENTRIES];
gdt_pointer_t gdt_ptr;

extern void gdt_flush(uint32_t gdt_ptr_addr);

void gdt_init() {
    gdt_ptr.base = (uint32_t) &gdt;
    gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_NUM_ENTRIES) - 1;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

    gdt_flush((uint32_t) &gdt_ptr);
}

void gdt_set_gate(uint32_t idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt[idx].base_low    = (base & 0xFFFF);
    gdt[idx].base_middle = (base >> 16) & 0xFF;
    gdt[idx].base_high   = (base >> 24) & 0xFF;

    gdt[idx].limit_low   = (limit & 0xFFFF);
    gdt[idx].granularity = (limit >> 16) & 0x0F;

    gdt[idx].granularity |= granularity & 0xF0;
    gdt[idx].access      = access;
}

