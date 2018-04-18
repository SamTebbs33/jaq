//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "print.h"
#include "gdt.h"
#include "idt.h"
#include "../driver/keyboard.h"
#include "../driver/timer.h"
#include "../fs/initrd.h"
#include "paging.h"
#include "multiboot.h"
#include "maths.h"

fs_node_t *fs_root;

void kmain(multiboot_info_t* mb_info) {
    uint32_t total_mem = mb_info->mem_upper + mb_info->mem_lower;
    uint32_t initrd_start = *(uint32_t*) mb_info->mods_addr;
    uint32_t initrd_end = *(uint32_t*) (mb_info->mods_addr + 4);

    gdt_init();
    idt_init();
    paging_init(total_mem, initrd_end);
    keyboard_init();
    timer_init(50);

    print_clear();
    print("Jaq OS\t");
    print_u32(total_mem / 1024);
    print("MB available\n");

    if(mb_info->mods_count == 1) {
        fs_root = initrd_init(initrd_start);
    }
    // TODO: Load drivers from initrd
}