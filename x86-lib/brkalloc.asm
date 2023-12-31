; +==============================================================+
; | x86-lib/brkalloc.asm -- Assembly source for allocator        |
; |                                                              |
; | (Heap-Allocator)                                             |
; +==============================================================+
;
; Implements two functions: alloc and dealloc
; 
; alloc:
;     * Moves the brk address to allocate more space on the heap.
;     * Generates header for the allocated chunk
; 
; dealloc:
;     * Sets chunk to free

global alloc
global dealloc

section .text
alloc:
    ; Allocates memory on the heap by moving the brk address.
    ; Adjusts allocation size as multiple of 32 bytes
    ; Generates header for the allocated chunk.
    ;
    ; Input:
    ;   * rdi: Size of allocation requested
    ; Output:
    ;   * rax: Address for beginning of data segment of allocated
    ;          chunk
    ;
    ; (Resets values of rbx, rdx, r8)

    enter 0, 0

    ; Store rdi for later use
    mov rdx, rdi
    mov r8, rdi

    ; Calling sys_sbrk to find current brk address
    mov rax, 12
    xor rdi, rdi
    syscall

    ; Storing brk address for later use
    mov rbx, rax

    ; Calculate appropriate size for the chunk
    ; [ chunk size = ((size + 0x18) // 32 + 1) * 32 ]
    add rdx, 0x18
    shr rdx, 5
    inc rdx
    shl rdx, 5

    ; Calling sys_sbrk to move brk address using calculated size
    mov rax, 12
    lea rdi, [rbx + rdx]
    syscall

    ; First qword for header
    ; ( status and allocation size )
    sub rdx, 0x18
    mov rdi, r8
    shl rdx, 8
    mov dl, 1
    mov [rbx], rdx

    ; Increment address of rbx
    add rbx, 0x8

    ; Second qword for header
    ; ( Next chunk address in heap )
    mov [rbx], rax

    ; Store data segment address in rax to return
    lea rax, [rbx + 0x10]
    ; Reset rbx, rdx, and r8
    xor rbx, rbx
    xor rdx, rdx
    xor r8, r8
    leave
    ret

dealloc:
    ; Marks allocated chunk to be free
    ; Makes changes to the header section alone (First qword)
    ;
    ; Input:
    ;   * rdi: Address of memory buffer
    ; Output:
    ;     (void)
    ;
    ; (Resets value of rax, rbx)

    enter 0, 0

    ; Calculate beginning of header
    lea rbx, [rdi - 0x18]
    ; Copy header into rax
    mov rax, [rbx]
    ; Set status byte to 0 (free)
    mov al, 0
    ; Write the header back to the chunk
    mov [rbx], rax

    ; Reset rax and rbx
    xor rax, rax
    xor rbx, rbx
    leave
    ret
