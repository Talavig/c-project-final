; symbol and branch

.extern ext_label

add:                        addi $1, 5, $2

DUPLABEL: move $1, $2
DUPLABEL:       sub $3, $4, $5

    bne $1, $2, ext_label      
    jmp UNDEFINED_TARGET   
    call add       
    hlt
