//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "screen/print.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "../driver/keyboard.h"
#include "../driver/timer.h"
#include "../driver/serial.h"
#include "../fs/initrd.h"
#include "mem/paging.h"
#include "multiboot.h"
#include "util/maths.h"
#include "mem/mem.h"
#include "screen/framebuffer.h"
#include <driver_ifc.h>

fs_node_t *fs_root;

driver_ifc_t driver_ifc = {
        .mem_ifc = {
                .kmalloc = kmalloc,
                .kmalloc_a = kmalloc_a,
                .kmalloc_p = kmalloc_p,
                .kmalloc_ap = kmalloc_ap
        },
        .fs_ifc = {
                .fs_readdir = fs_readdir,
                .fs_read = fs_read,
                .fs_finddir = fs_finddir,
                .fs_make_node = fs_make_node,
                .fs_close = fs_close,
                .fs_open = fs_open,
                .fs_write = fs_write
        },
        .fb_ifc = {
                .fb_cursor = fb_cursor,
                .fb_clear = fb_clear,
                .fb_putc = fb_putc
        },
        .int_ifc = {
                .interrupts_register_handler = interrupts_register_handler
        }
};

void kmain(multiboot_info_t* mb_info) {
    uint32_t total_mem = mb_info->mem_upper + mb_info->mem_lower;
    uint32_t initrd_start = *(uint32_t*) mb_info->mods_addr;
    uint32_t initrd_end = *(uint32_t*) (mb_info->mods_addr + 4);

    gdt_init();
    idt_init();
    paging_init(total_mem, initrd_end);
    keyboard_init();
    timer_init(50);
    serial_init(SERIAL_COM1_PORT, 38400, false, 8, true, false, 0);
    serial_write(SERIAL_COM1_PORT, 'j');

    print_clear();
    print("Jaq OS ");
    print_u32(total_mem / 1024);
    print("MB available\n");

    if(mb_info->mods_count == 1) {
        fs_root = initrd_init(initrd_start);
    }
    // TODO: Load drivers from initrd
}