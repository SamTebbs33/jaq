//
// Created by sam on 26/01/19.
//
#include <stdinc.h>
#include <arch.h>
#include <paging.h>
#include <gdt.h>
#include <idt.h>

extern void* kernel_stack;
void arch_init(uint32_t total_mem, uint32_t initrd_end, multiboot_info_t* mb_info) {
    gdt_init((uint32_t) & kernel_stack, 0x28);
    idt_init();
    paging_init(total_mem, initrd_end);
}
