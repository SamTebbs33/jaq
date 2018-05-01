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
#include "log/log.h"
#include <driver_ifc.h>
#include <syscalls.h>

fs_node_t *fs_root;

driver_ifc_t driver_ifc = {
        .mem_ifc = {
                .kmalloc = kmalloc,
                .kmalloc_a = kmalloc_a,
                //.kmalloc_p = kmalloc_p,
                //.kmalloc_ap = kmalloc_ap
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

syscall_handler_t syscall_handlers[SYSCALL_MAX + 1];
extern void* kernel_stack;

void handle_syscall(interrupt_registers_t registers) {
    uint8_t syscall = (uint8_t) (registers.eax & 0xFF);
    if(syscall_handlers[syscall]) syscall_handlers[syscall](registers);
    else logf(LOG_LEVEL_WARNING, "Unhandled syscall %d\n", syscall);
}

void syscall_test(interrupt_registers_t registers) {
    log_debug("Got test syscall\n");
}

void syscall_register_handler(uint8_t syscall, syscall_handler_t handler) {
    if(syscall >= SYSCALL_MAX) return;
    if(!syscall_handlers[syscall]) syscall_handlers[syscall] = handler;
    else logf(LOG_LEVEL_WARNING, "Attempted to re-register a handler for syscall %d\n", syscall);
}

void kmain(multiboot_info_t* mb_info) {
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

    log_info("Initialising paging\n");
    paging_init(total_mem, initrd_end);

    log_info("Initialising devices\n");
    keyboard_init();

    if(mb_info->mods_count == 1) {
        log_info("Loading initrd\n");
        fs_root = initrd_init(initrd_start);
        // TODO: Load drivers from initrd
    }

    interrupts_register_handler(SYSCALL_INTERRUPT, handle_syscall);
    syscall_register_handler(0x3, syscall_test);

    log_info("Done!");

    uint32_t fake_total_ram = total_mem - (total_mem % 1024) + 1024;
    print_clear();
    print("Jaq OS ");
    print_u32(fake_total_ram / 1024);
    print("MB available\n");

    asm("int $0x80");

    // Runs forever to make sure interrupts are handled
    while (true);
}