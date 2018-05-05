//
// Created by Samuel Tebbs on 12/04/2018.
//

#include <syscalls.h>
#include "idt.h"
#include "../util/string.h"
#include "../util/util.h"
#include "interrupts.h"
#include "../screen/print.h"

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

// Syscall handler
extern void irq96();

extern void idt_flush(uint32_t idt_ptr_addr);

bool handle(interrupt_registers_t registers);

idt_entry_t idt[IDT_NUM_ENTRIES];
idt_pointer_t idt_ptr;

interrupt_handler_t handlers[IDT_NUM_ENTRIES];

void idt_init() {
    idt_ptr.limit = sizeof(idt_entry_t) * IDT_NUM_ENTRIES - 1;
    idt_ptr.base  = (uint32_t) &idt;

    memset(idt, 0, sizeof(idt_entry_t) * IDT_NUM_ENTRIES);

    // Remap the PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    // ISRs
    idt_set_gate(0, (uint32_t) isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint32_t) isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint32_t) isr2, 0x08, 0x8E);
    idt_set_gate(3, (uint32_t) isr3, 0x08, 0x8E);
    idt_set_gate(4, (uint32_t) isr4, 0x08, 0x8E);
    idt_set_gate(5, (uint32_t) isr5, 0x08, 0x8E);
    idt_set_gate(6, (uint32_t) isr6, 0x08, 0x8E);
    idt_set_gate(7, (uint32_t) isr7, 0x08, 0x8E);
    idt_set_gate(8, (uint32_t) isr8, 0x08, 0x8E);
    idt_set_gate(9, (uint32_t) isr9, 0x08, 0x8E);
    idt_set_gate(10, (uint32_t) isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t) isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t) isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t) isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t) isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t) isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t) isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t) isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t) isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t) isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t) isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t) isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t) isr22, 0x08, 0x8E);
    idt_set_gate(23, (uint32_t) isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t) isr24, 0x08, 0x8E);
    idt_set_gate(25, (uint32_t) isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t) isr26, 0x08, 0x8E);
    idt_set_gate(27, (uint32_t) isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t) isr28, 0x08, 0x8E);
    idt_set_gate(29, (uint32_t) isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t) isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t) isr31, 0x08, 0x8E);

    // IRQs
    idt_set_gate(32, (uint32_t) irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t) irq1, 0x08, 0x8E);
    idt_set_gate(34, (uint32_t) irq2, 0x08, 0x8E);
    idt_set_gate(35, (uint32_t) irq3, 0x08, 0x8E);
    idt_set_gate(36, (uint32_t) irq4, 0x08, 0x8E);
    idt_set_gate(37, (uint32_t) irq5, 0x08, 0x8E);
    idt_set_gate(38, (uint32_t) irq6, 0x08, 0x8E);
    idt_set_gate(39, (uint32_t) irq7, 0x08, 0x8E);
    idt_set_gate(40, (uint32_t) irq8, 0x08, 0x8E);
    idt_set_gate(41, (uint32_t) irq9, 0x08, 0x8E);
    idt_set_gate(42, (uint32_t) irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t) irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t) irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t) irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t) irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t) irq15, 0x08, 0x8E);
    // Syscall interrupt
    idt_set_gate(96, (uint32_t) irq96, 0x08, 0x8E | 0x60);
    
    idt_flush((uint32_t) &idt_ptr);
}

void idt_set_gate(uint8_t idx, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[idx].base_lo = base & 0xFFFF;
    idt[idx].base_hi = (base >> 16) & 0xFFFF;

    idt[idx].sel     = sel;
    idt[idx].always0 = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idt[idx].flags   = flags /* | 0x60 */;
}

bool handle(interrupt_registers_t registers) {
    uint8_t interrupt = registers.int_no;
    if(handlers[interrupt]) {
        handlers[interrupt](registers);
        return true;
    }
    return false;
}

void isr_handler(interrupt_registers_t registers) {
    if(!handle(registers)) logf(LOG_LEVEL_ERR, "Unhandled exception %d\n", registers.int_no);
}

void irq_handler(interrupt_registers_t registers) {
    handle(registers);
    // If this interrupt is coming from the PIC
    if (registers.int_no <= 47) {
        // Acknowledge the PICs
        if (registers.int_no >= 40) outb(0xA0, 0x20); // Slave acknowledgment
        outb(0x20, 0x20); // Master acknowledgment
    }
}

bool interrupts_register_handler(uint8_t interrupt, interrupt_handler_t handler) {
    if(handlers[interrupt]) return true;
    handlers[interrupt] = handler;
    return false;
}