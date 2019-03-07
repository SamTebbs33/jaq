.extern tss
useresp:
    .long 0

# void arch_restore_cpu_state(arch_cpu_state_t* state) with state in eax
.global arch_restore_cpu_state
arch_restore_cpu_state:
    # Restore the general purpose registers
    mov 16(%eax), %edi
    mov 20(%eax), %esi
    mov 24(%eax), %ebp
    mov 32(%eax), %ebx
    mov 36(%eax), %edx
    mov 40(%eax), %ecx
    mov 44(%eax), %eax
    ret

# void arch_save_cpu_state(arch_cpu_state_t* state)
.global arch_save_cpu_state
arch_save_cpu_state:
    # Save caller ebp
    push %ebp
    # Use local frame
    mov %esp, %ebp
    # Save ecx
    push %ecx
    # Get state to save into from the arguments. This is the 4th entry on the stack
    mov 12(%esp), %ecx
    # Save general purpose registers
    mov %eax, 44(%ecx)
    mov %edx, 36(%ecx)
    mov %ebx, 32(%ecx)
    mov %ebp, 24(%ecx)
    mov %esi, 20(%ecx)
    mov %edi, 16(%ecx)
    # Pop off ecx into state
    pop 40(%ecx)
    # Clear local variables
    mov %ebp, %esp
    # Restore caller's ebp
    pop %ebp
    ret

# void arch_switch_to_kernel_task(arch_cpu_state_t* current, arch_cpu_state_t* next)
.global arch_switch_to_kernel_task
arch_switch_to_kernel_task:
    # Disable interrupts to avoid being interrupted mid-switch
    cli
    # Get the current proc's state
    mov 4(%esp), %edi
    # Get the next proc's state
    mov 8(%esp), %eax
    # Changes to arch_cpu_state_t may require adjustments to the offset here
    # Save the stack pointer into the current proc's state at arch_cpu_state_t.esp
    mov %esp, 28(%edi)
    mov %ebp, 24(%edi)
    # Restore the stack pointer from the next proc's state from arch_cpu_state_t.esp
    mov 28(%eax), %esp
    mov 24(%eax), %ebp
    mov %esp, (tss + 4)
    # From now on we are using the next proc's stack

    # Restore kernel cpu state, we shouldn't touch any registers after this
    call arch_restore_cpu_state

    # Re-enable interrupts
    sti
    # Return to return address stored at start of next proc's stack
    ret

# void arch_switch_to_user_task(arch_cpu_state_t* current, arch_cpu_state_t* next, arch_cpu_state_t* next_user)
.global arch_switch_to_user_task
arch_switch_to_user_task:
    # Disable interrupts to avoid being interrupted mid-switch
    cli
    # Get the current proc's state
    mov 4(%esp), %edi
    # Get the next proc's state
    mov 8(%esp), %esi
    # Get the next proc's user state
    mov 12(%esp), %eax
    # Changes to arch_cpu_state_t may require adjustments to the offset here
    # Save the stack pointer into the current proc's state at arch_cpu_state_t.esp
    mov %esp, 28(%edi)
    mov %ebp, 24(%edi)
    # Restore the stack pointer from the next proc's state from arch_cpu_state_t.esp
    mov 28(%esi), %esp
    mov 24(%esi), %ebp
    # From now on we are using the next proc's kernel stack
    mov %esp, (tss + 4)
    # Save user.esp
    mov 28(%eax), %ebx
    mov %ebx, (useresp)

    # Restore user cpu state, we shouldn't touch any registers after this
    call arch_restore_cpu_state

    # User data segment
    pushl $0x23
    # User stack pointer
    pushl (useresp)
    # EFLAGS with interrupts re-enabled
    pushfl
    orl $0x200, (%esp)
    # User code segment
    pushl $0x1B
    # Address to jump to, put on kernel stack by process initialiser
    push 16(%esp)

    iret

