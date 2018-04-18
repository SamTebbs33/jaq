//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "screen/print.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "../driver/keyboard.h"
#include "../driver/timer.h"
#include "../fs/initrd.h"
#include "mem/paging.h"
#include "multiboot.h"
#include "util/maths.h"
#include "mem/mem.h"

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
    print("Jaq OS ");
    print_u32(total_mem / 1024);
    print("MB available\n");

    if(mb_info->mods_count == 1) {
        fs_root = initrd_init(initrd_start);
    }
    // TODO: Load drivers from initrd
}