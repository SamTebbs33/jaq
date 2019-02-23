.extern tss
useresp:
    .long 0

# void arch_restore_cpu_state(arch_cpu_state_t* state)
.global arch_restore_cpu_state
arch_restore_cpu_state:
    # Get the state to restore
    pop %eax
    # Restore the general purpose registers
    mov 20(%eax), %edi
    mov 24(%eax), %esi
    mov 28(%eax), %ebp
    mov 36(%eax), %ebx
    mov 40(%eax), %edx
    mov 44(%eax), %ecx
    mov 48(%eax), %eax
    ret

# void arch_save_cpu_state(arch_cpu_state_t* state)
.global arch_save_cpu_state
arch_save_cpu_state:
    push %ecx
    # Get state to save into
    mov 4(%esp), %ecx
    # Save general purpose registers
    mov %eax, 48(%ecx)
    mov %edx, 40(%ecx)
    mov %ebx, 36(%ecx)
    mov %ebp, 28(%ecx)
    mov %esi, 24(%ecx)
    mov %edi, 20(%ecx)
    # Pop off ecx into state
    pop 44(%ecx)
    # Save stack pointer
    mov %esp, 32(%ecx)
    # Clear argument
    add %esp, 4
    ret

# void arch_switch_to_kernel_task(arch_cpu_state_t* current, arch_cpu_state_t* next)
.global arch_switch_to_kernel_task
arch_switch_to_kernel_task:
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

    # Restore kernel cpu state, we shouldn't touch any registers after this
    push %esi
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
    mov 20(%esp), %edi
    # Get the next proc's state
    mov 24(%esp), %esi
    # Get the next proc's user state
    mov 28(%esp), %edx
    # Changes to arch_cpu_state_t may require adjustments to the offset here
    # Save the stack pointer into the current proc's state at arch_cpu_state_t.esp
    mov %esp, 28(%edi)
    # Restore the stack pointer from the next proc's state from arch_cpu_state_t.esp
    mov 28(%esi), %esp
    # From now on we are using the next proc's kernel stack
    mov %esp, (tss + 4)
    # Save user.esp
    mov 28(%edx), %ebx
    mov %ebx, (useresp)

    # Set user data segment
    mov $0x23, %ebx
    mov %ebx, %ds
    mov %ebx, %es
    mov %ebx, %fs
    mov %ebx, %gs

    # Restore user cpu state, we shouldn't touch any registers after this
    push %edx
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

