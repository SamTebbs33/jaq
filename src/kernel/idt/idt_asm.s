.macro isr_no_err isr_num:req
    .global isr\isr_num
    isr\isr_num:
    cli
    push 0
    push \isr_num
    jmp isr_common
.endm

.macro isr_err isr_num
    .global isr\isr_num
    isr\isr_num:
    cli
    push \isr_num
    jmp isr_common
.endm

isr_no_err 0
isr_no_err 1
isr_no_err 2
isr_no_err 3
isr_no_err 4
isr_no_err 5
isr_no_err 6
isr_no_err 7
isr_err 8
isr_no_err 9
isr_err 10
isr_err 11
isr_err 12
isr_err 13
isr_err 14
isr_no_err 15
isr_no_err 16
isr_err 17
isr_no_err 18
isr_no_err 19
isr_no_err 20
isr_no_err 21
isr_no_err 22
isr_no_err 23
isr_no_err 24
isr_no_err 25
isr_no_err 26
isr_no_err 27
isr_no_err 28
isr_no_err 29
isr_err 30
isr_no_err 31

.extern isr_handler

/*
 This is the common ISR stub. It saves the processor state, sets
 up for kernel mode segments, calls the C-level fault handler,
 and finally restores the stack frame.
 */
isr_common:
   pusha

   mov %ds, %ax
   push %eax                 // save the data segment descriptor

   mov 0x10, %ax  // load the kernel data segment descriptor
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs

   call isr_handler

   pop %eax        // reload the original data segment descriptor
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs

   popa
   add 8, %esp     // Cleans up the pushed error code and pushed ISR number
   iret

.macro irq irq_num:req, int_num:req
  .global irq\irq_num
  irq\irq_num:
    cli
    push \irq_num
    push \int_num
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

.extern irq_handler

/*
 This is our common IRQ stub. It saves the processor state, sets
 up for kernel mode segments, calls the C-level fault handler,
 and finally restores the stack frame.
*/
irq_common:
   pusha

   mov %ds, %ax
   push %eax                 // save the data segment descriptor

   mov 0x10, %ax  // load the kernel data segment descriptor
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs

   call irq_handler

   pop %ebx        // reload the original data segment descriptor
   mov %bx, %ds
   mov %bx, %es
   mov %bx, %fs
   mov %bx, %gs

   popa
   add 8, %esp     // Cleans up the pushed error code and pushed IRQ number
   iret