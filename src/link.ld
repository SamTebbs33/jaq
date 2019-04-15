ENTRY(start)

KERNEL_ADDR_OFFSET = 0xC0000000;
KERNEL_VADDR_START = 0xC0100000;

SECTIONS
{

    . = 0x00100000;
    KERNEL_PHYSADDR_START = .;

    /* The entry symbol and higher-half set up code is placed in lower memory to avoid loading and execution issues */
    .multiboot.data : {
        *(.multiboot.data)
    }

    .multiboot.text : {
        *(.multiboot.text)
    }

    /* The kernel will be loaded at 1MB, but will be mapped to 3GB + 1MB  */
    . += KERNEL_ADDR_OFFSET;

    .text ALIGN(4K) : AT (ADDR (.text) - KERNEL_ADDR_OFFSET) {
        *(.text)
    }

    .rodata ALIGN(4K) : AT (ADDR (.rodata) - KERNEL_ADDR_OFFSET) {
        *(.rodata)
    }

    .data ALIGN(4K) : AT (ADDR (.data) - KERNEL_ADDR_OFFSET) {
        *(.data)
    }

	.bss ALIGN(4K) : AT (ADDR (.bss) - KERNEL_ADDR_OFFSET) {
        *(COMMON)
        *(.bss)
	}

    /DISCARD/ : {
        *(.eh_frame);
        *(.comment*);
    }

    KERNEL_PHYSADDR_END = . - KERNEL_ADDR_OFFSET;
    KERNEL_VADDR_END = .;
}
