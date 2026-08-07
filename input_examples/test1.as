.entry MAIN
.extern LOOP

mcro my_macro
    add $1, $2, $3
    sub $4, $5, $6
mcroend

MAIN: addi $1, $0, 5
      my_macro
      bne $1, $2, LOOP
      la LOOP
      hlt

STR:  .asciz "Hi"
ARR:  .dw 10, -20