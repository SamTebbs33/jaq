ALIGNMENT equ 1
MEM_INFO equ 2
MAGIC equ 0x1BADB002
FLAGS equ ALIGNMENT | MEM_INFO
CHECKSUM equ -(MAGIC + FLAGS)

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
global kernel_stack
extern kmain

start:
    mov esp, kernel_stack_end
    push ebx
    call kmain
    cli
.hlt:
    hlt
    jmp .hlt

global gdt_flush
KERNEL_CODE_SEGSEL equ 0x8
KERNEL_DATA_SEGSEL equ 0x10
TSS_SEGSEL equ 0x28

gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]

    mov ax, KERNEL_DATA_SEGSEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Enter protected mode
    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp KERNEL_CODE_SEGSEL:.flush
.flush:
    ret

global idt_flush

idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    sti
    ret

global tss_flush

tss_flush:
    mov ax, TSS_SEGSEL
    ltr ax
    ret