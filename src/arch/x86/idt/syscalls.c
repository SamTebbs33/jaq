//
// Created by sam on 05/02/19.
//

#include <syscalls.h>
#include <idt.h>
#include <arch_defs.h>
#include <screen/print.h>
#include <lib/util.h>

syscall_handler_t handlers[SYSCALL_NUM_HANDLERS];

void handle_syscall(arch_registers_t* regs) {
    uint32_t syscall = regs->eax;
    if(handlers[syscall]) handlers[syscall](regs);
}

void syscalls_init() {
    idt_register_isr_handler(128, handle_syscall);
}

void syscalls_register_handler(syscall_t syscall, syscall_handler_t handler) {
    if(syscall < SYSCALL_NUM_HANDLERS) handlers[syscall] = handler;
    else PANIC("Attempted to register out of bounds syscall handler: %d\n", syscall);
}

