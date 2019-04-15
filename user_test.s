.section .text
loop:
    mov %eax, %ebx
    mov %ebx, %eax
    jmp loop
