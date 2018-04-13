//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "print.h"
#include "gdt.h"
#include "idt.h"
#include "../drivers/keyboard.h"
#include "../drivers/timer.h"
#include "paging.h"

void kmain() {
    gdt_init();
    idt_init();
    paging_init(0x1000000); // Assume the RAM is 16MB big for now

    keyboard_init();
    timer_init(50);

    print_clear();
    PRINT("Hello, world!");
}