;valid test run
.extern extvar
.entry MAIN
.entry DATALABEL

mcro INIT_REGS
    add $1, $2, $3
    move $4, $5
mcroend

MAIN: addi $10, -15, $11
    INIT_REGS
    la extvar
    call SUBROUTINE
    bne $10, $11, LOOP
    jmp $7
    hlt

LOOP: lw $1, 12, $2
    sw $3, -4, $4
    lb $5, 0, $6
    sb $7, 2, $8
    lh $9, 8, $10
    sh $11, 4, $12
    bgt $1, $2, MAIN

SUBROUTINE: sub $15, $16, $17
    and $18, $19, $20
    or $21, $22, $23
    nor $24, $25, $26
    mvhi $2, $3
    mvlo $4, $5
    jmp $0

DATALABEL: .db 10, -20, 30
    .dh 1000, -2000
    .dw 65536, -100000
STR: .asciz "Hello, Assembler!"
