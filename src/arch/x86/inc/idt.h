//
// Created by samteb01 on 24/01/19.
//

#ifndef JAQ_IDT_H
#define JAQ_IDT_H

#include <stdinc.h>
#include <arch_types.h>

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
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_init();
void idt_register_irq_handler(int irq, arch_interrupt_handler_t handler);
void idt_register_isr_handler(int isr, arch_interrupt_handler_t handler);

#endif //JAQ_IDT_H
