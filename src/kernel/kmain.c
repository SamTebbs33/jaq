//
// Created by Samuel Tebbs on 12/04/2018.
//

#include <screen/print.h>
#include <arch.h>
#include <driver/keyboard.h>
#include <driver/timer.h>
#include <driver/serial.h>
#include <fs/initrd.h>
#include <multiboot.h>
#include <lib/maths.h>
#include <lib/util.h>
#include <mem/mem.h>
#include <screen/framebuffer.h>
#include <log/log.h>
#include <boot_defs.h>
#include <multitasking/multitasking.h>
#include <screen/print.h>
#include <arch_defs.h>
#include <lib/sorted_linkedlist.h>

fs_node_t *fs_root;
extern void* KERNEL_VADDR_END, * KERNEL_VADDR_START, * KERNEL_PHYSADDR_END, * KERNEL_PHYSADDR_START;
extern void* kernel_stack, * kernel_stack_end;

void kmain(multiboot_info_t* mb_info, uint32_t mb_magic) {
    ASSERT_EQ_INT("multiboot magic number", mb_magic, MULTIBOOT_BOOTLOADER_MAGIC);
    serial_init(SERIAL_COM1_PORT, 38400, false, 8, true, false, 0);
    uint32_t kernel_stack_size = (uint32_t)&kernel_stack_end - (uint32_t)&kernel_stack;

    uint32_t total_mem = mb_info->mem_upper + mb_info->mem_lower;
    uint32_t initrd_start = *((uint32_t*)mb_info->mods_addr);
    uint32_t initrd_end = *((uint32_t*)(mb_info->mods_addr + 4));
    uint32_t initrd_virtual_start = PHYSICAL_TO_VIRTUAL(initrd_start);
    uint32_t initrd_virtual_end = PHYSICAL_TO_VIRTUAL(initrd_end);

    log_info("Initialising arch (" ARCH ")\n");
    arch_init(total_mem, mb_info, (uint32_t) &KERNEL_VADDR_START, (uint32_t) &KERNEL_VADDR_END, (uint32_t) &KERNEL_PHYSADDR_START, (uint32_t) &KERNEL_PHYSADDR_END, initrd_end);

    log_info("Initialising filesytem\n");
    fs_init();
    if(mb_info->mods_count == 1) {
        initrd_init(initrd_virtual_start);
    }

    log_info("Initialising devices\n");
    keyboard_init();
    timer_init(TIMER_FREQUENCY);

    log_info("Initialising multitasking\n");
    multitasking_init((void *) &kernel_stack, kernel_stack_size);

    log_info("Done\n");

    uint32_t fake_total_ram = total_mem - (total_mem % 1024) + 1024;
//    printf("JaqOS on " ARCH ", %d MB available\n> ", fake_total_ram / 1024);

    // Runs forever to make sure interrupts are handled
    while (true) {
        multitasking_yield();
    }
}
