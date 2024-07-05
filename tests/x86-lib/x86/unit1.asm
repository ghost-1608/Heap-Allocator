;`{in: None, out: '5'}`

; Unit test - 1
; =============
;    * Allocates storage to store single char (1 byte)
;    * Stores heap buffer address
;    * Moves ASCII code for number "5" to the buffer
;    * Uses sys_write to print the char (from buffer)
;    * De-allocates memory

extern alloc, dealloc

%define CHAR_SIZE 0x1
    
global _start

section .bss
    buffer RESQ 1
    
section .text
_start:
    ; Call alloc
    mov rdi, CHAR_SIZE
    call alloc

    ; Store buffer address returned
    mov [buffer], rax

    ; Move '5' to the buffer
    mov rax, 53
    mov rbx, [buffer]
    mov [rbx], rax

    ; Prepare and call sys_write to print buffer
    mov rax, 1
    mov rdi, 1
    mov rsi, rbx
    mov rdx, 0x1
    syscall

    ; Call dealloc
    mov rdi, [buffer]
    call dealloc

    ; Exit
    mov rax, 60
    xor rdi, rdi
    syscall
