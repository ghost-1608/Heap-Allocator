; +==============================================================+
; | x86-lib/brkalloc.asm -- Assembly source for allocator        |
; |                                                              |
; | (Heap-Allocator)                                             |
; +==============================================================+
;
; Implements two functions: alloc and dealloc
; 
; alloc:
;     * Moves the program break to allocate more space on the heap.
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
    ;   * RDI: Size of allocation requested
    ; Output:
    ;   * RAX: Address for beginning of data section of allocated
    ;          chunk
    ;
    ; (Resets values of RBX, RDX, R10)

    enter 0, 0

    xor rax, rax

    ; Store size for later use
    mov rdx, rdi
    mov r10, rdi

    ; Invalid size handle (zero)
    test rdi, rdi
    jz .exit    
    

    ; Calling sys_brk to find current break address
    mov rax, 12
    xor rdi, rdi
    syscall

    ; Storing break address for later use
    mov rbx, rax

    ; Calculate appropriate size for the chunk
    ; [ chunk size = ((size + 0x18) // 32 + 1) * 32 ]
    add rdx, 0x18
    shr rdx, 5
    inc rdx
    shl rdx, 5

    ; Calling sys_brk to move break address using calculated size
    mov rax, 12
    lea rdi, [rbx + rdx]
    syscall

    ; Error handling (negative value)
    test rax, rax
    js .exit

    ; First qword for header
    ; ( status and allocation size )
    sub rdx, 0x18
    shl rdx, 8
    mov dl, 1
    mov [rbx], rdx

    ; Increment address of RBX
    add rbx, 0x8

    ; Second qword for header
    ; ( Next chunk address in heap )
    mov [rbx], rax

    ; Store data section address in RAX to return
    lea rax, [rbx + 0x10]
.exit:
    ; Restore size value to RDI
    mov rdi, r10
    ; Reset RBX, RDX, and R10
    xor rbx, rbx
    xor rdx, rdx
    xor r10, r10
    leave
    ret

dealloc:
    ; Marks allocated chunk to be free
    ; Makes changes to the header section alone (First qword)
    ;
    ; Input:
    ;   * RDI: Address of memory buffer
    ; Output:
    ;     (void)
    ;
    ; (Resets value of RBX)

    enter 0, 0

    ; Calculate beginning of header
    lea rbx, [rdi - 0x18]
    ; Set status byte to 0 (free)
    mov [rbx], byte 0

    ; Reset RBX
    xor rbx, rbx
    leave
    ret
