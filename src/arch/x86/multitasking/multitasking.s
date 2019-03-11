# void arch_switch_task(arch_cpu_state_t* current, arch_cpu_state_t* next)
.global arch_switch_task
arch_switch_task:
    # Disable interrupts to avoid being interrupted mid-switch
    cli
    # Push current values so that they are popped off when we switch back to the current task
    # The other general purpose registers are pushed by the C calling convention
    push %ebx
    push %esi
    push %ebp
    push %edi
    # Get the current proc's state
    mov 20(%esp), %edi
    # Get the next proc's state
    mov 24(%esp), %esi
    # Changes to arch_cpu_state_t may require adjustments to the offset here
    # Save the stack pointer into the current proc's state at arch_cpu_state_t.esp
    mov %esp, 28(%edi)
    # Restore the stack pointer from the next proc's state from arch_cpu_state_t.esp
    mov 28(%esi), %esp
    # From now on we are using the next proc's stack

    # Pop off saved values from next proc's stack
    # The other general purpose registers are popped by the C calling convention
    pop %edi
    pop %ebp
    pop %esi
    pop %ebx
    # Re-enable interrupts
    sti
    # Return to return address stored at start of next proc's stack
    ret
