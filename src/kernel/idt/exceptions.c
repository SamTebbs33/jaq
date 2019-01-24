//
// Created by sam on 05/05/18.
//

#include <idt.h>
#include <exceptions.h>
#include <util.h>

void divide_by_zero(registers_t* registers) {
    PANIC("Divide by zero");
}

void debug(registers_t* registers) {
    PANIC("Debug");
}

void non_maskable_interrupt(registers_t* registers) {
    PANIC("Non maskable interrupt");
}

void breakpoint(registers_t* registers) {
    PANIC("Breakpoint");
}

void overflow(registers_t* registers) {
    PANIC("Overflow");
}

void bound_range_exceeded(registers_t* registers) {
    PANIC("Bound range exceeded");
}

void invalid_opcode(registers_t* registers) {
    PANIC("Invalid opcode");
}

void device_not_available(registers_t* registers) {
    PANIC("Device not available");
}

void double_fault(registers_t* registers) {
    PANIC("Double fault");
}

void invalid_tss(registers_t* registers) {
    PANIC("Invalid TSS");
}

void segment_not_present(registers_t* registers) {
    PANIC("Segment not present");
}

void stack_segment_fault(registers_t* registers) {
    PANIC("Stack-segment fault");
}

void general_protection_fault(registers_t* registers) {
    PANIC("General protection fault");
}

void page_fault(registers_t* registers) {
    PANIC("Page fault");
}

void reserved(registers_t* registers) {
    PANIC("Reserved");
}

void floating_point_exception(registers_t* registers) {
    PANIC("x87 floating point exception");
}

void alignment_check(registers_t* registers) {
    PANIC("Alignment check");
}

void machine_check(registers_t* registers) {
    PANIC("Machine check");
}

void simd_floating_point_exception(registers_t* registers) {
    PANIC("SIMD floating point exception");
}

void virtualization_exception(registers_t* registers) {
    PANIC("Virtualization exception");
}

void reserved2(registers_t* registers) {
    PANIC("Reserved 2");
}

void security_exception(registers_t* registers) {
    PANIC("Security exception");
}

void reserved3(registers_t* registers) {
    PANIC("Reserved 3");
}

void exceptions_init() {
    idt_register_isr_handler(EXCEPTION_DIV_BY_ZERO, divide_by_zero);
    idt_register_isr_handler(EXCEPTION_DEBUG, debug);
    idt_register_isr_handler(EXCEPTION_NON_MASKABLE_INT, non_maskable_interrupt);
    idt_register_isr_handler(EXCEPTION_BREAKPOINT, breakpoint);
    idt_register_isr_handler(EXCEPTION_OVERFLOW, overflow);
    idt_register_isr_handler(EXCEPTION_BOUND_RANGE_EXCEEDED, bound_range_exceeded);
    idt_register_isr_handler(EXCEPTION_INVALID_OPCODE, invalid_opcode);
    idt_register_isr_handler(EXCEPTION_DEVICE_NOT_AVAILABLE, device_not_available);
    idt_register_isr_handler(EXCEPTION_DOUBLE_FAULT, double_fault);
    idt_register_isr_handler(EXCEPTION_INVALID_TSS, invalid_tss);
    idt_register_isr_handler(EXCEPTION_SEGMENT_NOT_PRESENT, segment_not_present);
    idt_register_isr_handler(EXCEPTION_STACK_SEGMENT_FAULT, stack_segment_fault);
    idt_register_isr_handler(EXCEPTION_GENERAL_PROTECTION_FAULT, general_protection_fault);
    idt_register_isr_handler(EXCEPTION_PAGE_FAULT, page_fault);
    idt_register_isr_handler(EXCEPTION_RESERVED, reserved);
    idt_register_isr_handler(EXCEPTION_x87_FP, floating_point_exception);
    idt_register_isr_handler(EXCEPTION_ALIGNMENT_CHECK, alignment_check);
    idt_register_isr_handler(EXCEPTION_MACHINE_CHECK, machine_check);
    idt_register_isr_handler(EXCEPTION_SIMD_FP, simd_floating_point_exception);
    idt_register_isr_handler(EXCEPTION_VIRTUALIZATION, virtualization_exception);
    idt_register_isr_handler(EXCEPTION_RESERVED2, reserved2);
    idt_register_isr_handler(EXCEPTION_SECURITY_EXCEPTION, security_exception);
    idt_register_isr_handler(EXCEPTION_RESERVED3, reserved3);
}
