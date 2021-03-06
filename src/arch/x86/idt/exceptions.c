//
// Created by sam on 05/05/18.
//

#include <idt.h>
#include <exceptions.h>
#include <lib/util.h>

#define EXCEPTION_HANDLER(name, string) void name(arch_cpu_state_t* regs) {\
    uint32_t cr0, cr2, cr3;\
    asm("mov %%cr0, %0": "=r"(cr0));\
    asm("mov %%cr2, %0": "=r"(cr2));\
    asm("mov %%cr3, %0": "=r"(cr3));\
    PANIC(string "\n"\
        "\tcs=0x%x\tss=0x%x\tgs=0x%x\tfs=0x%x\tes=0x%x\tds=0x%x\n"\
        "\tebp=0x%x\tesp=0x%x\n"\
        "\tedi=0x%x\tesi=0x%x\tebx=0x%x\tedx=0x%x\tecx=0x%x\teax=0x%x\n"\
        "\tint=0x%x\t\terr=0x%x\n"\
        "\teip=0x%x\tef=0x%x\tuesp=0x%x\n"\
        "\tcr0=0x%x\tcr2=0x%x\tcr3=0x%x\n"\
    , regs->cs, regs->ss, regs->gs, regs->fs, regs->es, regs->ds, regs->ebp, regs->esp, regs->edi, regs->esi, regs->ebx, regs->edx, regs->ecx, regs->eax, regs->int_no, regs->err_code, regs->eip, regs->eflags, regs->useresp, cr0, cr2, cr3);\
}
#define REGISTER_EXCEPTION_HANDLER(isr, name) idt_register_isr_handler(isr, name)

EXCEPTION_HANDLER(exception_divide_by_zero, "Division by zero")
EXCEPTION_HANDLER(exception_debug, "Debug")
EXCEPTION_HANDLER(exception_nmi, "Non-maskable interrupt")
EXCEPTION_HANDLER(exception_breakpoint, "Breakpoint")
EXCEPTION_HANDLER(exception_overflow, "Overflow")
EXCEPTION_HANDLER(exception_bound_range_exceeded, "Bound range exceeded")
EXCEPTION_HANDLER(exception_invalid_opcode, "Invalid opcode")
EXCEPTION_HANDLER(exception_device_not_available, "Device not available")
EXCEPTION_HANDLER(exception_double_fault, "Double fault")
EXCEPTION_HANDLER(exception_invalid_tss, "Invalid TSS")
EXCEPTION_HANDLER(exception_segment_not_present, "Segment not present")
EXCEPTION_HANDLER(exception_stack_segment_fault, "Stack segment fault")
EXCEPTION_HANDLER(exception_gpe, "General protection fault")
EXCEPTION_HANDLER(exception_page_fault, "Page fault")
EXCEPTION_HANDLER(exception_reserved, "Reserved exception")
EXCEPTION_HANDLER(exception_fpe, "Floating point exception")
EXCEPTION_HANDLER(exception_alignment_check, "Alignment check exception")
EXCEPTION_HANDLER(exception_machine_check, "Machine check")
EXCEPTION_HANDLER(exception_simd_fpe, "SIMD floating point exception")
EXCEPTION_HANDLER(exception_virtualisation, "Virtualisation exception")
EXCEPTION_HANDLER(exception_reserved2, "Reserved2 exception")
EXCEPTION_HANDLER(exception_security, "Security exception")
EXCEPTION_HANDLER(exception_reserved3, "Reserved3 exception")

void exceptions_init() {
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_DIV_BY_ZERO, exception_divide_by_zero);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_DEBUG, exception_debug);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_NON_MASKABLE_INT, exception_nmi);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_BREAKPOINT, exception_breakpoint);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_OVERFLOW, exception_overflow);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_BOUND_RANGE_EXCEEDED, exception_bound_range_exceeded);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_INVALID_OPCODE, exception_invalid_opcode);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_DEVICE_NOT_AVAILABLE, exception_device_not_available);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_DOUBLE_FAULT, exception_double_fault);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_INVALID_TSS, exception_invalid_tss);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_SEGMENT_NOT_PRESENT, exception_segment_not_present);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_STACK_SEGMENT_FAULT, exception_stack_segment_fault);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_GENERAL_PROTECTION_FAULT, exception_gpe);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_PAGE_FAULT, exception_page_fault);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_RESERVED, exception_reserved);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_x87_FP, exception_fpe);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_ALIGNMENT_CHECK, exception_alignment_check);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_MACHINE_CHECK, exception_machine_check);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_SIMD_FP, exception_simd_fpe);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_VIRTUALIZATION, exception_virtualisation);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_RESERVED2, exception_reserved2);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_SECURITY_EXCEPTION, exception_security);
    REGISTER_EXCEPTION_HANDLER(EXCEPTION_RESERVED3, exception_reserved3);
}
