; +==============================================================+
; | x64-lib/brkalloc.asm -- Assembly source for allocator        |
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
    ; Generates header for the allocated chunk.
    ;
    ; Input:
    ;   * RDI: Size of allocation requested
    ; Output:
    ;   * RAX: Address for beginning of data section of allocated
    ;          chunk
    ;
    ; (Resets values of RDX)

    ; Invalid size handle (zero)
    test rdi, rdi
    jz .exit    

    ; Store size for later use
    mov rdx, rdi
    
    ; Calling sys_brk to find current brk address
    mov eax, 12
    xor edi, edi
    syscall

    ; Calculate new address for brk
    ; ( current brk address + size + header )
    lea rdi, [rax + rdx + 0x10]

    ; Calling sys_brk to move break address using calculated size
    mov eax, 12
    syscall

    ; Error handling (negative value)
    test rax, rax
    js .exit

    ; First qword for header
    ; ( Allocation size )
    mov [rax], rdx

    ; Second qword for header
    ; ( Flags = F# (8 Flag Bytes) )
    ; ( F0 F1 F2 F3 F4 F5 F6 F7 )
    ; ( F7 -> Usage status (0x00 - Free, 0x01 - Used) )
    mov byte [rax + 0xf], 0x01

    ; Store data section address in RAX to return
    add rax, 0x10

.exit:
    ; Restore size value to RDI
    mov rdi, rdx
    ; Reset RDX
    xor edx, edx

    ret

dealloc:
    ; Marks allocated chunk to be free
    ; Makes changes to the header section alone (Second qword)
    ;
    ; Input:
    ;   * RDI: Address of memory buffer
    ; Output:
    ;     (void)
    ;

    ; Change the usage status flag byte to 0x00 (Free)

    test rdi, rdi
    jz .exit

    mov byte [rdi - 0x1], 0x00

.exit:
    ret
