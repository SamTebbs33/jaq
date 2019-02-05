//
// Created by samteb01 on 24/01/19.
//

#ifndef JAQ_IDT_H
#define JAQ_IDT_H

#include <stdinc.h>
#include <arch_defs.h>

struct idt_entry {
    unsigned short base_low;
    unsigned short sel;
    unsigned char zero;
    unsigned char flags;
    unsigned short base_high;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    uintptr_t base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void idt_load();
/**
 * Set the flags for a certain IDT entry
 * @param num The IDT entry
 * @param base The address of the handler for this IDT entry
 * @param sel The code segment selector that the handler belongs to
 * @param flags The entry's flags. See spec for details
 */
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

/**
 * Initialise and load the IDT as well as the exception handlers
 */
void idt_init();

/**
 * Register an IRQ handler
 * @param irq The IRQ to handle
 * @param handler The handler
 */
void idt_register_irq_handler(int irq, arch_interrupt_handler_t handler);

/**
 * Register an ISR handler
 * @param isr The ISR to handle
 * @param handler The handler
 */
void idt_register_isr_handler(int isr, arch_interrupt_handler_t handler);

#endif //JAQ_IDT_H
