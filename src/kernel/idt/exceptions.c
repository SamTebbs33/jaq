//
// Created by sam on 05/05/18.
//

#include <interrupts.h>
#include "exceptions.h"
#include "../util/util.h"

void divide_by_zero(interrupt_registers_t registers) {
    PANIC("Divide by zero");
}

void debug(interrupt_registers_t registers) {
    PANIC("Debug");
}

void non_maskable_interrupt(interrupt_registers_t registers) {
    PANIC("Non maskable interrupt");
}

void breakpoint(interrupt_registers_t registers) {
    PANIC("Breakpoint");
}

void overflow(interrupt_registers_t registers) {
    PANIC("Overflow");
}

void bound_range_exceeded(interrupt_registers_t registers) {
    PANIC("Bound range exceeded");
}

void invalid_opcode(interrupt_registers_t registers) {
    PANIC("Invalid opcode");
}

void device_not_available(interrupt_registers_t registers) {
    PANIC("Device not available");
}

void double_fault(interrupt_registers_t registers) {
    PANIC("Double fault");
}

void invalid_tss(interrupt_registers_t registers) {
    PANIC("Invalid TSS");
}

void segment_not_present(interrupt_registers_t registers) {
    PANIC("Segment not present");
}

void stack_segment_fault(interrupt_registers_t registers) {
    PANIC("Stack-segment fault");
}

void general_protection_fault(interrupt_registers_t registers) {
    PANIC("General protection fault");
}

void page_fault(interrupt_registers_t registers) {
    PANIC("Page fault");
}

void reserved(interrupt_registers_t registers) {
    PANIC("Reserved");
}

void floating_point_exception(interrupt_registers_t registers) {
    PANIC("x87 floating point exception");
}

void alignment_check(interrupt_registers_t registers) {
    PANIC("Alignment check");
}

void machine_check(interrupt_registers_t registers) {
    PANIC("Machine check");
}

void simd_floating_point_exception(interrupt_registers_t registers) {
    PANIC("SIMD floating point exception");
}

void virtualization_exception(interrupt_registers_t registers) {
    PANIC("Virtualization exception");
}

void reserved2(interrupt_registers_t registers) {
    PANIC("Reserved 2");
}

void security_exception(interrupt_registers_t registers) {
    PANIC("Security exception");
}

void reserved3(interrupt_registers_t registers) {
    PANIC("Reserved 3");
}

void exceptions_init() {
    interrupts_register_handler(EXCEPTION_DIV_BY_ZERO, divide_by_zero);
    interrupts_register_handler(EXCEPTION_DEBUG, debug);
    interrupts_register_handler(EXCEPTION_NON_MASKABLE_INT, non_maskable_interrupt);
    interrupts_register_handler(EXCEPTION_BREAKPOINT, breakpoint);
    interrupts_register_handler(EXCEPTION_OVERFLOW, overflow);
    interrupts_register_handler(EXCEPTION_BOUND_RANGE_EXCEEDED, bound_range_exceeded);
    interrupts_register_handler(EXCEPTION_INVALID_OPCODE, invalid_opcode);
    interrupts_register_handler(EXCEPTION_DEVICE_NOT_AVAILABLE, device_not_available);
    interrupts_register_handler(EXCEPTION_DOUBLE_FAULT, double_fault);
    interrupts_register_handler(EXCEPTION_INVALID_TSS, invalid_tss);
    interrupts_register_handler(EXCEPTION_SEGMENT_NOT_PRESENT, segment_not_present);
    interrupts_register_handler(EXCEPTION_STACK_SEGMENT_FAULT, stack_segment_fault);
    interrupts_register_handler(EXCEPTION_GENERAL_PROTECTION_FAULT, general_protection_fault);
    interrupts_register_handler(EXCEPTION_PAGE_FAULT, page_fault);
    interrupts_register_handler(EXCEPTION_RESERVED, reserved);
    interrupts_register_handler(EXCEPTION_x87_FP, floating_point_exception);
    interrupts_register_handler(EXCEPTION_ALIGNMENT_CHECK, alignment_check);
    interrupts_register_handler(EXCEPTION_MACHINE_CHECK, machine_check);
    interrupts_register_handler(EXCEPTION_SIMD_FP, simd_floating_point_exception);
    interrupts_register_handler(EXCEPTION_VIRTUALIZATION, virtualization_exception);
    interrupts_register_handler(EXCEPTION_RESERVED2, reserved2);
    interrupts_register_handler(EXCEPTION_SECURITY_EXCEPTION, security_exception);
    interrupts_register_handler(EXCEPTION_RESERVED3, reserved3);
}
