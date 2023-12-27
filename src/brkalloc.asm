global alloc
global dealloc

section .text
alloc:
    enter 0, 0

    mov rdx, rdi
    mov r8, rdi
    
    mov rax, 12
    xor rdi, rdi
    syscall

    mov rbx, rax

    mov rax, 12
    lea rdx, [rdx + 0x18]
    shr rdx, 5
    inc rdx
    shl rdx, 5
    lea rdi, [rbx + rdx]
    syscall

    lea rdx, [rdx - 0x18]
    mov rdi, r8
    shl rdx, 8
    mov dl, 1

    mov [rbx], rdx
    lea rbx, [rbx + 0x8]
    mov [rbx], rax

    lea rax, [rbx + 0x10]
    xor rbx, rbx
    xor rdx, rdx
    leave
    ret

dealloc:
    enter 0, 0

    lea rbx, [rdi - 24]
    mov rax, [rbx]
    mov al, 0
    mov [rbx], rax

    xor rbx, rbx
    leave
    ret
