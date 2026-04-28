global my_strcmp

section     .text

;***********************************************************************************************
; My strcmp function
;
; Expects:
;        RDI - address of the first word structure
;        RSI - address of the second word structure
; Returns in RAX:
;        0 - if comparing strings are equal
;        1 - otherwise
; Must save:
;        RBX, RSP, RBP, R12-R15
; Can spoil:
;        RAX, RCX, RDX, RSI, RDI, R8-R11
;***********************************************************************************************
my_strcmp:
        ; get the addresses of the beginning of the strings
        mov r8, [rdi]
        mov r9, [rsi]

        vmovdqa ymm0, [r8]   
        vmovdqa ymm1, [r9]

        ; compare
        vpcmpeqb ymm2, ymm0, ymm1
        
        ; get mask
        vpmovmskb eax, ymm2

        ; check for equality
        cmp eax, 0xFFFFFFFF
        je .equal

.not_equal:
        mov rax, 1
        vzeroupper           ; Clear the state of the AVX registers
        ret

.equal:
        xor rax, rax
        vzeroupper
        ret

section .note.GNU-stack noalloc noexec nowrite progbits