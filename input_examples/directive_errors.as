; data directive errors

MAIN: .db 5,,10
    .db ,5, 10     
    .db 5, 10,   
    .dh 99999             
    .db -200 
    
STRERR1: .asciz "Missing closing quote
STRERR2: .asciz "Too" "Many" Quotes
    hlt
