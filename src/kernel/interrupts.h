//
// Created by Samuel Tebbs on 12/04/2018.
//

#ifndef JAQ_INTERRUPTS_H
#define JAQ_INTERRUPTS_H

#include "stdbool.h"
#include "stdint.h"

#define IRQ_0 32
#define IRQ_1 33
#define IRQ_2 34
#define IRQ_3 35
#define IRQ_4 36
#define IRQ_5 37
#define IRQ_6 38
#define IRQ_7 39
#define IRQ_8 40
#define IRQ_9 41
#define IRQ_10 42
#define IRQ_11 43
#define IRQ_12 44
#define IRQ_13 45
#define IRQ_14 46
#define IRQ_15 47

#define ISR_0 0
#define ISR_1 1
#define ISR_2 2
#define ISR_3 3
#define ISR_4 4
#define ISR_5 5
#define ISR_6 6
#define ISR_7 7
#define ISR_8 8
#define ISR_9 9
#define ISR_10 10
#define ISR_11 11
#define ISR_12 12
#define ISR_13 13
#define ISR_14 14
#define ISR_15 15
#define ISR_16 16
#define ISR_17 17
#define ISR_18 18
#define ISR_19 19
#define ISR_20 20
#define ISR_21 21
#define ISR_22 22
#define ISR_23 23
#define ISR_24 24
#define ISR_25 25
#define ISR_26 26
#define ISR_27 27
#define ISR_28 28
#define ISR_29 29
#define ISR_30 30
#define ISR_31 31

struct interrupt_registers {
    uint32_t ds;                  // Data segment selector
    uint32_t edi, esi, ebp, ignored, ebx, edx, ecx, eax; // Pushed by pusha.
    uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
    uint32_t eip, cs, eflags, esp, ss; // Pushed by the processor automatically.
} __attribute__((packed));
typedef struct interrupt_registers interrupt_registers_t;

typedef void (*interrupt_handler_t)(interrupt_registers_t);

bool interrupts_register_handler(uint8_t interrupt, interrupt_handler_t handler);

#endif //JAQ_INTERRUPTS_H
