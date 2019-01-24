//
// Created by samteb01 on 24/01/19.
//

#ifndef JAQ_IDT_H
#define JAQ_IDT_H

#include <stdinc.h>

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

struct registers {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};
typedef struct registers registers_t;

typedef void (*interrupt_handler_t) (registers_t*);

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void idt_load();
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_init();
void idt_register_irq_handler(int irq, interrupt_handler_t handler);
void idt_register_isr_handler(int isr, interrupt_handler_t handler);

#endif //JAQ_IDT_H
