//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "print.h"
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"

void kmain() {
    gdt_init();
    idt_init();

    keyboard_init();
    print_clear();
    PRINT("Hello, world!");
}