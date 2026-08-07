; file: logic_valid.as
; Testing full logic, branches, and valid symbols

.entry  MAIN
.entry  LOOP1
.extern EXTVAR

mcro DOMATH
    add $2, $3, $4
    sub $4, $2, $5
mcroend

MAIN:   la      EXTVAR
        jmp     LOOP1

LOOP1:  move    $3, $4 
        DOMATH
        bne     $5, $4, MAIN
        
        DOMATH
        la      MYDATA
        call    EXTVAR
        
ENDPROG: hlt

MYDATA: .asciz  "LogicTest"
NUMS:   .db     +12, -45, 0, 100
        .dw     -123456, +987654
        .dh     32767, -32768
