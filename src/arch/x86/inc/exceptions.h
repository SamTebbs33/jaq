//
// Created by sam on 05/05/18.
//

#ifndef JAQ_EXCEPTIONS_H
#define JAQ_EXCEPTIONS_H

#define EXCEPTION_PRINT_STATE(regs) \
    uint32_t cr0, cr2, cr3;\
    asm("mov %%cr0, %0": "=r"(cr0));\
    asm("mov %%cr2, %0": "=r"(cr2));\
    asm("mov %%cr3, %0": "=r"(cr3));\
    logf(LOG_LEVEL_ERR,\
        "\tcs=0x%x\tss=0x%x\tgs=0x%x\tfs=0x%x\tes=0x%x\tds=0x%x\n"\
        "\tebp=0x%x\tesp=0x%x\n"\
        "\tedi=0x%x\tesi=0x%x\tebx=0x%x\tedx=0x%x\tecx=0x%x\teax=0x%x\n"\
        "\tint=0x%x\t\terr=0x%x\n"\
        "\teip=0x%x\tef=0x%x\tuesp=0x%x\n"\
        "\tcr0=0x%x\tcr2=0x%x\tcr3=0x%x\n"\
    , regs->cs, regs->ss, regs->gs, regs->fs, regs->es, regs->ds, regs->ebp, regs->esp, regs->edi, regs->esi, regs->ebx, regs->edx, regs->ecx, regs->eax, regs->int_no, regs->err_code, regs->eip, regs->eflags, regs->useresp, cr0, cr2, cr3);\

#define EXCEPTION_DIV_BY_ZERO 0
#define EXCEPTION_DEBUG 1
#define EXCEPTION_NON_MASKABLE_INT 2
#define EXCEPTION_BREAKPOINT 3
#define EXCEPTION_OVERFLOW 4
#define EXCEPTION_BOUND_RANGE_EXCEEDED 5
#define EXCEPTION_INVALID_OPCODE 6
#define EXCEPTION_DEVICE_NOT_AVAILABLE 7
#define EXCEPTION_DOUBLE_FAULT 8
#define EXCEPTION_INVALID_TSS 10
#define EXCEPTION_SEGMENT_NOT_PRESENT 11
#define EXCEPTION_STACK_SEGMENT_FAULT 12
#define EXCEPTION_GENERAL_PROTECTION_FAULT 13
#define EXCEPTION_PAGE_FAULT 14
#define EXCEPTION_RESERVED 15
#define EXCEPTION_x87_FP 16
#define EXCEPTION_ALIGNMENT_CHECK 17
#define EXCEPTION_MACHINE_CHECK 18
#define EXCEPTION_SIMD_FP 19
#define EXCEPTION_VIRTUALIZATION 20
#define EXCEPTION_RESERVED2 21
#define EXCEPTION_SECURITY_EXCEPTION 30
#define EXCEPTION_RESERVED3 31

/**
 * Initialise the exception handlers. Requires the IDT to have been set up already.
 */
void exceptions_init();

#endif //JAQ_EXCEPTIONS_H
