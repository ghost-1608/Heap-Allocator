;`{in: None, out: 1024}`

; Unit test - 2
; =============
;    * Allocates storage to store 32-bit integer (4 bytes)
;    * Stores heap buffer address
;    * Moves int32 number "1024" to the buffer
;    * Uses print2::push_int32_as_ASCII to print the number (from buffer)
;    * De-allocates memory

%include "../../../x86-lib/brkalloc.asm"
%include "../../../../print2.asm"

%define INT32_SIZE 0x4
    
global _start

section .rodata
    newline DB 10
    
section .bss
    buffer RESQ 1
    
section .text
_start:
    ; Call alloc
    mov rdi, INT32_SIZE
    call alloc

    ; Store buffer address returned
    mov [buffer], rax

    ; Move 1024 to the buffer
    mov rax, 1024
    mov rbx, [buffer]
    mov [rbx], rax

    xor r11, r11

    ; Add newline
    mov rdi, newline
    mov rsi, 1
    call push_ASCII

    ; Add buffer contents
    mov rdi, [buffer]
    mov rdi, [rdi]
    call push_int32_as_ASCII

    ; Prepare and call sys_write to print
    mov rax, 1
    mov rdi, 1
    lea rsi, [rsp + rbx]
    mov rdx, r11
    syscall    
    
    ; Call dealloc
    mov rdi, [buffer]
    call dealloc

    ; Exit
    mov rax, 60
    xor rdi, rdi
    syscall
