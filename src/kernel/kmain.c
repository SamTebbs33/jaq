//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "screen/print.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "idt/exceptions.h"
#include "../driver/keyboard.h"
#include "../driver/timer.h"
#include "../driver/serial.h"
#include "../fs/initrd.h"
#include "mem/paging.h"
#include "multiboot.h"
#include "util/maths.h"
#include "util/util.h"
#include "mem/mem.h"
#include "screen/framebuffer.h"
#include "log/log.h"

fs_node_t *fs_root;
extern void* kernel_stack;

void kmain(multiboot_info_t* mb_info, uint32_t mb_magic) {
    ASSERT_EQ_INT("multiboot magic number", mb_magic, MULTIBOOT_BOOTLOADER_MAGIC);
    serial_init(SERIAL_COM1_PORT, 38400, false, 8, true, false, 0);
    print_clear();

    log_info("Parsing multiboot info\n");
    uint32_t total_mem = mb_info->mem_upper + mb_info->mem_lower;
    uint32_t initrd_start = *(uint32_t*) mb_info->mods_addr;
    uint32_t initrd_end = *(uint32_t*) (mb_info->mods_addr + 4);

    log_info("Initialising GDT\n");
    gdt_init((uint32_t) &kernel_stack, 0x28);
    log_info("Initialising IDT\n");
    idt_init();
    exceptions_init();

    log_info("Initialising paging\n");
    paging_init(total_mem, initrd_end);

    log_info("Initialising devices\n");
    keyboard_init();

    if(mb_info->mods_count == 1) {
        log_info("Loading initrd\n");
        fs_root = initrd_init(initrd_start);
        // TODO: Load drivers from initrd
    }

    log_info("Initialising timer\n");
    timer_init(10);

    log_info("Done!\n");

    uint32_t fake_total_ram = total_mem - (total_mem % 1024) + 1024;
    print_clear();
    printf("JaqOS, %d MB available\n> ", fake_total_ram / 1024);

    // Runs forever to make sure interrupts are handled
    while (true);
}
