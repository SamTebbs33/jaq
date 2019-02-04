//
// Created by sam on 26/01/19.
//
#include <stdinc.h>
#include <arch.h>
#include <paging.h>
#include <gdt.h>
#include <idt.h>

extern void* kernel_stack;

void arch_init(uint32_t total_mem, multiboot_info_t* mb_info, uint32_t virtual_start, uint32_t virtual_end, uint32_t phys_start, uint32_t phys_end, uint32_t initrd_end) {
    gdt_init((uint32_t) &kernel_stack, 0x28);
    idt_init();
    paging_init(total_mem, virtual_start, virtual_end, phys_start, phys_end, initrd_end);
}
