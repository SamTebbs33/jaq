# void arch_save_cpu_state(arch_cpu_state_t* state)
.global arch_save_cpu_state
arch_save_cpu_state:
    # Save twice
    push %eax
    push %eax
    # State to store into
    mov 12(%esp), %eax
    mov %edi, 16(%eax)
    mov %esi, 20(%eax)
    mov %ebx, 32(%eax)
    mov %edx, 36(%eax)
    mov %ecx, 40(%eax)
    # Save pushed eax into state
    pop 44(%eax)
    # Restore eax to what caller expects it to be
    pop %eax
    ret

# void arch_restore_cpu_state(arch_cpu_state_t* state)
.global arch_restore_cpu_state
arch_restore_cpu_state:
    mov 4(%esp), %eax
    mov 16(%eax), %edi
    mov 20(%eax), %esi
    mov 32(%eax), %ebx
    mov 36(%eax), %edx
    mov 40(%eax), %ecx
    mov 44(%eax), %eax
    ret

# void arch_switch_task(arch_cpu_state_t* current, arch_cpu_state_t* next)
.global arch_switch_task
arch_switch_task:
    # Disable interrupts to avoid being interrupted mid-switch
    cli
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

    push %esi
    call arch_restore_cpu_state
    add $4, %esp
    # Re-enable interrupts
    sti
    # Return to return address stored at start of next proc's stack
    ret
