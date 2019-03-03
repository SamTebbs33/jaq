.set ALIGNMENT, 1
.set MEM_INFO, 2
.set MAGIC, 0x1BADB002
.set FLAGS, (ALIGNMENT | MEM_INFO)
.set CHECKSUM, -(MAGIC + FLAGS)

.set KERNEL_CODE_SEGMENT, 0x8
.set KERNEL_DATA_SEGMENT, 0x10
.set KERNEL_TSS_SEGMENT, 0x28
.set KERNEL_ADDR_OFFSET, 0xC0000000
.set KERNEL_PAGE_NUMBER, KERNEL_ADDR_OFFSET >> 22
# One for kernel code, one for temporary allocation space and 5 for the heap. Each covers 4MiB
.set KERNEL_NUM_UPPER_PAGES, 7

# Multiboot data put in lower memory
.section .multiboot.data, "a"
.align 4
.long  MAGIC
.long  FLAGS
.long  CHECKSUM

.align 4096
# The initial page directory used to boot into the higher half
# Allocates seven 4MB pages for kernel, one for the initial 4MB, and six in the upper 4MB for the kernel code, kernel pile and
# kernel heap
boot_page_directory:
    # Set lower half pages. This will be unmapped later on as we'll be jumping out of the lower 4MiB
    .long 0x00000083
    # Fill all preceding pages with 4 bytes
    .fill (KERNEL_PAGE_NUMBER - 1), 4
    # Set higher half pages, starting from 0 and increasing by 4MiB
    .long 0x00000083
    .long 0x00000083 | (1 << 22)
    .long 0x00000083 | (2 << 22)
    .long 0x00000083 | (3 << 22)
    .long 0x00000083 | (4 << 22)
    .long 0x00000083 | (5 << 22)
    .long 0x00000083 | (6 << 22)
     # Fill all succeeding pages with 4 bytes
    .fill (1024 - KERNEL_PAGE_NUMBER - KERNEL_NUM_UPPER_PAGES), 4

# Multiboot code put in lower memory
.section .multiboot.text, "ax", @progbits
.align 16

.global start
start:
    # Load in the page directory
    mov $boot_page_directory, %ecx
    mov %ecx, %cr3

    mov %cr4, %ecx
    # Enable 4MB pages
    or $0x00000010, %ecx
    mov %ecx, %cr4

    # Enable paging
    mov %cr0, %ecx
    or $0x80000000, %ecx
    mov %ecx, %cr0
    jmp start_higher_half

.section .text
.global kernel_stack
.extern kmain

start_higher_half:
    # Unmap the identity mapping of the first 4MB
	invlpg (0)

	# Set up the stack
    mov $kernel_stack_end, %esp

    # Push magic number from bootloader
    push %eax
    # Push multiboot header address. with the virtual offset
    add $KERNEL_ADDR_OFFSET, %ebx
    push %ebx

    cli
    call kmain
loop:    jmp loop

.global gdt_flush
gdt_flush:
    mov 4(%esp), %eax
    lgdt (%eax)

    mov $KERNEL_DATA_SEGMENT, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    # Enter protected mode
    mov %cr0, %eax
    or $0x1, %al
    mov %eax, %cr0

    jmp $KERNEL_CODE_SEGMENT,$flush
flush:
    ret

.global idt_flush
idt_flush:
    mov 4(%esp), %eax
    lidt (%eax)
    ret

.global tss_flush
tss_flush:
    mov $KERNEL_TSS_SEGMENT, %ax
    ltr %ax
    ret

.section .bss
.align 16

.global kernel_stack
.global kernel_stack_end
kernel_stack:
    .fill 16384
kernel_stack_end:
