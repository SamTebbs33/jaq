.set ALIGNMENT, 1
.set MEM_INFO, 2
.set MAGIC, 0x1BADB002
.set FLAGS, (ALIGNMENT | MEM_INFO)
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long  MAGIC
.long  FLAGS
.long  CHECKSUM

.section .bss
.align 16
kernel_stack:
    .fill 16384
kernel_stack_end:

.section .text
.global start
.global kernel_stack
.extern kmain

start:
    mov $kernel_stack_end, %esp
    # Push magic number from bootloader
    push %eax
    # Push multiboot header address
    push %ebx
    cli
    call kmain
loop:    jmp loop

.global gdt_flush
.set KERNEL_CODE_SEGSEL, 0x8
.set KERNEL_DATA_SEGSEL, 0x10
.set TSS_SEGSEL, 0x28

gdt_flush:
    mov 4(%esp), %eax
    lgdt (%eax)

    mov $KERNEL_DATA_SEGSEL, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    # Enter protected mode
    mov %cr0, %eax
    or $0x1, %al
    mov %eax, %cr0

    jmp $KERNEL_CODE_SEGSEL,$flush
flush:
    ret

.global idt_flush

idt_flush:
    mov 4(%esp), %eax
    lidt (%eax)
    ret

.global tss_flush

tss_flush:
    mov $TSS_SEGSEL, %ax
    ltr %ax
    ret
