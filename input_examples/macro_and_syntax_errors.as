; macro and syntax errors
mcro add     
    add $1, $2, $3
mcroend

mcro VALID_MACRO extra_text
    move $1, $2
mcroend

mcro END_MACRO
    hlt
mcroend extra_tokens_here

MAIN:
    VALID_MACRO
    add $1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, $20
    hlt
