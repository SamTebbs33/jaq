//
// Created by Samuel Tebbs on 12/04/2018.
//

#ifndef JAQ_GDT_H
#define JAQ_GDT_H

#include <stdinc.h>

#define GDT_NUM_ENTRIES 6

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

struct tss_entry {
    uint16_t prev_task_link;
    uint16_t reserved;
    uint32_t esp0;
    uint16_t ss0;
    uint16_t reserved0;
    uint32_t esp1;
    uint16_t ss1;
    uint16_t reserved1;
    uint32_t esp2;
    uint16_t ss2;
    uint16_t reserved2;

    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;

    uint16_t es;
    uint16_t reserved3;
    uint16_t cs;
    uint16_t reserved4;
    uint16_t ss;
    uint16_t reserved5;
    uint16_t ds;
    uint16_t reserved6;
    uint16_t fs;
    uint16_t reserved7;
    uint16_t gs;
    uint16_t reserved8;

    uint16_t ldt_ss;
    uint16_t reserved9;

    uint16_t debug_and_reserved; /* The lowest bit is for debug */
    uint16_t io_map_base;

} __attribute__((packed));
typedef struct tss_entry tss_t;

void gdt_init(uint32_t kernel_stack_vaddr, uint16_t segment_selector);
void gdt_set_entry(uint32_t idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);
void gdt_set_tss_entry(uint32_t idx, uint32_t tss_vaddr);

#endif //JAQ_GDT_H
