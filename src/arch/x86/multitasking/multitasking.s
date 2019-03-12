.extern tss
useresp:
    .long 0
usereip:
    .long 0

# void arch_switch_to_kernel_task(arch_cpu_state_t* current, arch_cpu_state_t* next)
.global arch_switch_to_kernel_task
arch_switch_to_kernel_task:
    # Disable interrupts to avoid being interrupted mid-switch
    cli
    push %edi
    push %eax
    # Get the current proc's state
    mov 12(%esp), %edi
    # Get the next proc's state
    mov 16(%esp), %eax

    # Save current state
    pop 44(%edi)
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
    mov %esp, (tss + 4)

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
    # Save user.esp to temp storage
    mov 28(%eax), %ebx
    mov %ebx, (useresp)
    # Save user.eip to temp storage
    mov 56(%eax), %ebx
    mov %ebx, (usereip)

    push %eax
    # Restore user cpu state, we shouldn't touch any registers after this
    #call arch_restore_cpu_state
    add $4, %esp

    # User data segment
    pushl $0x23
    # User stack pointer
    pushl (useresp)
    # EFLAGS with interrupts re-enabled
    pushl $0x202
    # User code segment
    pushl $0x1B
    # Address to jump to
    push (usereip)

    iret

