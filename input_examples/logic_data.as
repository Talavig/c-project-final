; file: logic_data.as
MAIN:   la      DATA1
        jmp     STR

DATA1:  .dw     2147483647, -2147483648
; Error: Out of bounds for 16-bit
DATA2:  .dh     32768, -32769
; Error: Out of bounds for 8-bit
DATA3:  .db     128, -129

STR:    .asciz  "Testing"

        add     $1, $2, $3
        ; Valid syntactically, calls a data segment
        call    DATA1
        hlt
