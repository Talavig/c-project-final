; file: logic_errors.as
.extern EXTLBL

MAIN:   add     $1, $2, $3
        ; Error: UNDEFINED is never declared
        beq     $1, $2, UNDEFINED
        ; Error: Conditional branch to external label
        bne     $1, $2, EXTLBL

DUPLICATE: move $1, $2
; Error: Duplicate label defined
DUPLICATE: move $3, $4

; Error: Entry declared for a label that doesn't exist
.entry  MISSING

ENDPROG: hlt
