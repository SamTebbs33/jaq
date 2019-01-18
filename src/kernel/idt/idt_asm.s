.macro ISR_NO_ERR isr_num
    .global isr\isr_num
    isr\isr_num:
    cli
    push 0
    push \isr_num
    jmp isr_common
.endm

.macro ISR_ERR isr_num
    .global isr\isr_num
    isr\isr_num:
    cli
    push \isr_num
    jmp isr_common
.endm

ISR_NO_ERR 0
ISR_NO_ERR 1
ISR_NO_ERR 2
ISR_NO_ERR 3
ISR_NO_ERR 4
ISR_NO_ERR 5
ISR_NO_ERR 6
ISR_NO_ERR 7
ISR_ERR 8
ISR_NO_ERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NO_ERR 15
ISR_NO_ERR 16
ISR_ERR 17
ISR_NO_ERR 18
ISR_NO_ERR 19
ISR_NO_ERR 20
ISR_NO_ERR 21
ISR_NO_ERR 22
ISR_NO_ERR 23
ISR_NO_ERR 24
ISR_NO_ERR 25
ISR_NO_ERR 26
ISR_NO_ERR 27
ISR_NO_ERR 28
ISR_NO_ERR 29
ISR_ERR 30
ISR_NO_ERR 31

.extern isr_handler

 # This is the common ISR stub. It saves the processor state, sets
 # up for kernel mode segments, calls the C-level fault handler,
 # and finally restores the stack frame.

isr_common:
   pusha

   mov %ds, %ax
   push %eax                 # save the data segment descriptor

   mov $0x10, %ax             # load the kernel data segment descriptor
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs

   call isr_handler

   pop %eax                  # reload the original data segment descriptor
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs

   popa
   add $8, %esp               # Cleans up the pushed error code and pushed ISR number
   iret

.macro IRQ irq_num b
  .global irq\irq_num
  irq\irq_num:
    cli
    push $\irq_num
    push $\b
    jmp irq_common
.endm

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47
IRQ 96, 128

.extern irq_handler

# This is our common IRQ stub. It saves the processor state, sets
# up for kernel mode segments, calls the C-level fault handler,
# and finally restores the stack frame.

irq_common:
   pusha

   mov %ds, %ax
   push %eax                 # save the data segment descriptor

   mov $0x10, %ax             # load the kernel data segment descriptor
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs

   call irq_handler

   pop %ebx                  # reload the original data segment descriptor
   mov %bx, %ds
   mov %bx, %es
   mov %bx, %fs
   mov %bx, %gs

   popa
   add $8, %esp               # Cleans up the pushed error code and pushed IRQ number
   iret
