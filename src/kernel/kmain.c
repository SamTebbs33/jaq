//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "print.h"
#include "gdt.h"

void kmain() {
    gdt_init();
    print_clear();
    PRINT("Hello, world!");
}