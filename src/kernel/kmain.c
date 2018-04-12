//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "print.h"
#include "gdt.h"
#include "idt.h"

void kmain() {
    gdt_init();
    idt_init();

    print_clear();
    PRINT("Hello, world!");

    asm volatile ("int $0x3");
    asm volatile ("int $0x4");
}