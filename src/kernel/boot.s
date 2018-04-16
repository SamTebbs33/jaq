ALIGNMENT   equ 1
MEM_INFO    equ 2
MAGIC       equ 0x1BADB002
FLAGS       equ ALIGNMENT | MEM_INFO
CHECKSUM    equ -(MAGIC + FLAGS)

bits 32

section .multiboot
align 4
    dd  MAGIC
    dd  FLAGS
    dd  CHECKSUM

section .bss
align 16
kernel_stack:
    resb 16384
kernel_stack_end:

section .text
global start
extern kmain

start:
    mov esp, kernel_stack_end
    push    ebx
    cli
    call kmain
    jmp $

global gdt_flush

gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Enter protected mode
    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp 0x08:.flush
.flush:
    ret

global idt_flush

idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    sti
    ret