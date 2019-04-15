.extern tss

# void arch_switch_to_kernel_task(arch_cpu_state_t* current, arch_cpu_state_t* next)
.global switch_to_kernel_task
switch_to_kernel_task:
    # Disable interrupts to avoid being interrupted mid-switch
    cli
    push %edi
    push %eax
    # Get the current proc's state
    mov 12(%esp), %edi
    # Get the next proc's state
    mov 16(%esp), %eax

    # Save current kernel state
    # Save pushed eax into state
    pop 44(%edi)
    # Save pushed edi into state
    pop 16(%edi)
    mov %esi, 20(%edi)
    mov %ebp, 24(%edi)
    mov %esp, 28(%edi)
    mov %ebx, 32(%edi)
    mov %edx, 36(%edi)
    mov %ecx, 40(%edi)

    # Restore next state
    mov 16(%eax), %edi
    mov 20(%eax), %esi
    mov 24(%eax), %ebp
    mov 28(%eax), %esp
    mov 32(%eax), %ebx
    mov 36(%eax), %edx
    mov 40(%eax), %ecx
    mov 44(%eax), %eax
    # Use next proc's kernel esp when an interrupt occurs
    mov %esp, (tss + 4)

    # Re-enable interrupts
    sti
    # Return to return address stored at start of next proc's stack
    ret

# void arch_switch_to_user_task(arch_cpu_state_t* current, arch_cpu_state_t* next, uint32_t kernel_stack_base)
.global switch_to_user_task
switch_to_user_task:
    cli
    push %edi
    push %eax
    # Current proc's state
    mov 12(%esp), %edi
    # Next proc's state
    mov 16(%esp), %eax

    # Save current kernel state
    # Save pushed eax into state
    pop 48(%edi)
    # Save pushed edi into state
    pop 20(%edi)
    mov %esi, 24(%edi)
    mov %ebp, 28(%edi)
    mov %esp, 32(%edi)
    mov %ebx, 36(%edi)
    mov %edx, 40(%edi)
    mov %ecx, 44(%edi)
    mov %cr3, %ebx
    mov %ebx, (%edi)
    # Use base of next proc's kernel stack as esp for interrupts
    mov 12(%esp), %ebx
    mov %ebx, (tss + 4)
    # Restore next kernel state
    mov (%eax), %ebx
    mov %ebx, %cr3
    mov 20(%eax), %edi
    mov 24(%eax), %esi
    mov 28(%eax), %ebp
    mov 32(%eax), %esp
    mov 36(%eax), %ebx
    mov 40(%eax), %edx
    mov 44(%eax), %ecx
    mov 48(%eax), %eax

    # Interrupts are re-enabled by the interrupt handler
    # Return to interrupt handler
    ret

