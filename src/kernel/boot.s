.set ALIGNMENT, 1
.set MEM_INFO, 2
.set MAGIC, 0x1BADB002
.set FLAGS, ALIGNMENT | MEM_INFO
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
    .long  MAGIC
    .long  FLAGS
    .long  CHECKSUM

.section .bss
.align 16
kernel_stack:
    .skip 16384
kernel_stack_end:

.section .text
.global start
.extern kmain

start:
    mov $kernel_stack_end, %esp
    push %ebx
    call kmain
    cli
.hlt:
    hlt
    jmp .hlt

.global gdt_flush

gdt_flush:
    mov -4(%esp), %eax
    lgdt (%eax)

    mov 0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    // Enter protected mode
    mov %cr0, %eax
    or 1, %al
    mov %eax, %cr0

    ljmp $0x08, $.flush
.flush:
    ret

.global idt_flush

idt_flush:
    mov -4(%esp), %eax
    lidt (%eax)
    sti
    ret
