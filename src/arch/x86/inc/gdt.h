//
// Created by Samuel Tebbs on 12/04/2018.
//

#ifndef JAQ_GDT_H
#define JAQ_GDT_H

#include <stdinc.h>

#define GDT_NUM_ENTRIES 6

#define GDT_TYPE_TSS 0
#define GDT_TYPE_CODE 1
#define GDT_TYPE_DATA 1

#define GDT_LEVEL_KERNEL 0
#define GDT_LEVEL_USER 3

#define GDT_CODE_READABLE 1
#define GDT_CODE_NOT_READABLE 0
#define GDT_DATA_WRITABLE 1
#define GDT_DATA_NOT_WRITABLE 0
#define GDT_TSS_READWRITE 0
#define GDT_TSS_DIRCONF 0

#define GDT_CONF_ANY_LEVEL 1
#define GDT_CONF_THIS_LEVEL 0

#define GDT_DIR_UP 0
#define GDT_DIR_DOWN 1

#define GDT_SIZE_32BIT 1
#define GDT_SIZE_16BIT 0

#define GDT_GRANULARITY_BYTE 0
#define GDT_GRANULARITY_4KB 1

#define GDT_TSS_SEGMENT 0x28
#define GDT_CODE_SEGMENT 0x8
#define GDT_DATA_SEGMENT 0x10
#define GDT_CODE_SEGMENT_KERNEL (GDT_CODE_SEGMENT | GDT_LEVEL_KERNEL)
#define GDT_CODE_SEGMENT_USER ((GDT_CODE_SEGMENT + 0x10) | GDT_LEVEL_USER)
#define GDT_DATA_SEGMENT_KERNEL (GDT_DATA_SEGMENT | GDT_LEVEL_KERNEL)
#define GDT_DATA_SEMGENT_USER ((GDT_DATA_SEGMENT + 0x10) | GDT_LEVEL_USER)

struct gdt_entry
{
    uint16_t limit_low;
    uint32_t base_low : 24;
    uint8_t accessed : 1;
    uint8_t read_write : 1;
    uint8_t dir_conf : 1;
    uint8_t code : 1;
    uint8_t type : 1;
    uint8_t level : 2;
    uint8_t present : 1;
    uint8_t limit_high : 4;
    uint8_t available : 1;
    uint8_t always_0 : 1;
    uint8_t size : 1;
    uint8_t granularity : 1;
    uint8_t base_high;
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

/**
 * Initialise the GDT's entries
 * @param kernel_stack_vaddr The virtual address of the kernel stack. Gets inserted into the TSS
 * @param segment_selector The kernel segment selector. Gets inserted into the TSS
 */
void gdt_init(uint32_t kernel_stack_vaddr, uint32_t kernel_stack_size, uint16_t segment_selector);

/**
 * Set the flags of the corresponding GDT entry
 * @param idx The index into the GDT
 * @param base The base address that this segment applies to
 * @param limit The end address that this segment applies to
 * @param access The access flag. See spec for details
 * @param granularity The granularity flag. See spec for details
 */
void gdt_set_entry(uint32_t idx, uint32_t base, uint32_t limit, uint8_t read_write, uint8_t dir_conf, uint8_t code,
                   uint8_t type, uint8_t level, uint8_t size, uint8_t granularity, uint8_t present, uint8_t accessed, uint8_t available);

#endif //JAQ_GDT_H
