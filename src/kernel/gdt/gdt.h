//
// Created by Samuel Tebbs on 12/04/2018.
//

#ifndef JAQ_GDT_H
#define JAQ_GDT_H

#include <stdinc.h>

#define GDT_NUM_ENTRIES 5

struct gdt_entry
{
    uint16_t limit_low;           // The lower 16 bits of the limit.
    uint16_t base_low;            // The lower 16 bits of the base.
    uint8_t  base_middle;         // The next 8 bits of the base.
    uint8_t  access;              // Access flags, determine what ring this segment can be used in.
    uint8_t  granularity;
    uint8_t  base_high;           // The last 8 bits of the base.
} __attribute__((packed));
typedef struct gdt_entry gdt_entry_t;

struct gdt_pointer
{
    uint16_t limit;               // The upper 16 bits of all selector limits.
    uint32_t base;                // The address of the first gdt_entry_t struct.
} __attribute__((packed));

typedef struct gdt_pointer gdt_pointer_t;

void gdt_init();
void gdt_set_gate(uint32_t idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

#endif //JAQ_GDT_H
