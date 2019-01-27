//
// Created by Samuel Tebbs on 12/04/2018.
//

#include <print.h>
#include <arch.h>
#include <keyboard.h>
#include <timer.h>
#include <serial.h>
#include <initrd.h>
#include <multiboot.h>
#include <maths.h>
#include <util.h>
#include <mem.h>
#include <framebuffer.h>
#include <log.h>

fs_node_t *fs_root;

void kmain(multiboot_info_t* mb_info, uint32_t mb_magic) {
    ASSERT_EQ_INT("multiboot magic number", mb_magic, MULTIBOOT_BOOTLOADER_MAGIC);
    serial_init(SERIAL_COM1_PORT, 38400, false, 8, true, false, 0);
    print_clear();

    uint32_t total_mem = mb_info->mem_upper + mb_info->mem_lower;
    uint32_t initrd_start = *(uint32_t*) mb_info->mods_addr;
    uint32_t initrd_end = *(uint32_t*) (mb_info->mods_addr + 4);

    log_info("Init arch\n");
    arch_init(total_mem, initrd_end, mb_info);

    if(mb_info->mods_count == 1) {
        log_info("Init initrd\n");
        fs_root = initrd_init(initrd_start);
        // TODO: Load drivers from initrd
    }

    log_info("Init devices\n");
    keyboard_init();

    log_info("Init timer\n");
    timer_init(10);

    log_info("Done!\n");

    uint32_t fake_total_ram = total_mem - (total_mem % 1024) + 1024;
    print_clear();
    printf("JaqOS on " ARCH ", %d MB available\n> ", fake_total_ram / 1024);

    // Runs forever to make sure interrupts are handled
    while (true);
}
