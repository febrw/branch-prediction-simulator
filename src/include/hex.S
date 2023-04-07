/// --- Helper Macros --- ///

.macro PACKED_BYTE X
  .rept 16
    .byte \X
  .endr
.endm

.macro RSEQ_BYTE X
  .set I, 0
  .rept 16
    .byte \X - I
    .set I, I + 1
  .endr
.endm

.macro PACKED_WORD X
  .rept 8
    .word \X
  .endr
.endm

/// --- Data --- ///

.data

.align 16
rev:  RSEQ_BYTE 15

.align 16
lut0: PACKED_BYTE ('a' - 1)

.align 16
lut1: PACKED_BYTE '0'

.align 16
lut2: PACKED_BYTE ('a' - 10 - '0')

.align 16
lut3: PACKED_WORD 0x00ff

.align 16
lut4: PACKED_BYTE 0x30

.align 16
lut5:
    .word 100
    .word 10
    .word 1
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0

/// --- Code --- ///

.text
.align 16
.globl hex2ull
hex2ull:
    movdqu (%rdi), %xmm0
    pshufb rev(%rip), %xmm0
    movdqa %xmm0, %xmm1

    pcmpgtb lut0(%rip), %xmm1
    pand lut2(%rip), %xmm1

    psubb lut1(%rip), %xmm0
    psubb %xmm1, %xmm0

    movdqa %xmm0, %xmm1
    psrlw $4, %xmm1

    por %xmm1, %xmm0
    pand lut3(%rip), %xmm0
    packuswb %xmm0, %xmm0

    movq %xmm0, %rax
    ret

.align 16
.globl dec2ull
dec2ull:
    movq (%rdi), %xmm0
    pxor %xmm1, %xmm1

    psubb lut4(%rip), %xmm0
    punpcklbw %xmm1, %xmm0
    pmaddwd lut5(%rip), %xmm0
    phaddd %xmm0, %xmm0

    movq %xmm0, %rax
    ret
