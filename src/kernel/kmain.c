//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "print.h"
#include "gdt.h"
#include "idt.h"
#include "../driver/keyboard.h"
#include "../driver/timer.h"
#include "paging.h"
#include "multiboot.h"

void kmain(multiboot_info_t* mb_info) {
    uint32_t total_mem = mb_info->mem_upper + mb_info->mem_lower;

    gdt_init();
    idt_init();
    paging_init(total_mem, UINT32_MAX);
    keyboard_init();
    timer_init(50);

    print_clear();
    print("jaq OS, ");
    print_u32(total_mem / 1024);
    print("MB available");
}