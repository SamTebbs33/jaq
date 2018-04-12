//
// Created by Samuel Tebbs on 12/04/2018.
//

#ifndef JAQ_IDT_H
#define JAQ_IDT_H

#include "stdint.h"

#define IDT_NUM_ENTRIES 256

struct idt_entry {
    uint16_t base_lo;             // The lower 16 bits of the address to jump to when this interrupt fires.
    uint16_t sel;                 // Kernel segment selector.
    uint8_t always0;             // This must always be zero.
    uint8_t flags;               // More flags. See documentation.
    uint16_t base_hi;             // The upper 16 bits of the address to jump to.
} __attribute__((packed));
typedef struct idt_entry idt_entry_t;

// A struct describing a pointer to an array of interrupt handlers.
// This is in a format suitable for giving to 'lidt'.
struct idt_pointer {
    uint16_t limit;
    uint32_t base;                // The address of the first element in our idt_entry_t array.
} __attribute__((packed));
typedef struct idt_pointer idt_pointer_t;

void idt_init();
void idt_set_gate(uint8_t idx, uint32_t base, uint16_t sel, uint8_t flags);

#endif //JAQ_IDT_H
