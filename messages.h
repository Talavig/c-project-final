#ifndef MESSAGES_H
#define MESSAGES_H

#define ASM_ERROR(line_num, args) \
    do { \
        printf("Error in line %d: ", line_num); \
        printf args; \
        printf("\n"); \
    } while(0)

#define ASM_WARNING(line_num, args) \
    do { \
        printf("Warning in line %d: ", line_num); \
        printf args; \
        printf("\n"); \
    } while(0)


#define NO_ARGUMENTS_PASSED "Please pass at least one argument for assembler."
#define WRONG_FILE_EXTENTION "You may only pass files with .as to the assembler, please rename your file."

#define ERR_ALLOCATION_FAILED "Internal Error: Memory allocation failed for filenames.\n"
#define ERR_CANNOT_OPEN_FILE "Cannot open assembly file %s\n"
#define ERR_CANNOT_CREATE_FILE "Cannot create macro file %s\n"

#define ERR_LINE_TOO_LONG "Line exceeds maximum length of 80 characters."
#define ERR_EXTRA_CHARS_MACRO_END "Extra characters after 'mcroend'."
#define ERR_INVALID_MACRO_DEF "Invalid macro definition: '%s'."

#define ERR_MACRO_ALLOC_FAILED "Internal Error: Memory allocation failed for new macro.\n"
#define ERR_MACRO_NAME_ALLOC_FAILED "Internal Error: Memory allocation failed for macro name.\n"
#define ERR_MACRO_CONTENT_ALLOC_FAILED "Internal Error: Memory allocation failed for macro content.\n"
#define ERR_MACRO_CONTENT_REALLOC_FAILED "Internal Error: Memory reallocation failed for macro content.\n"


#define ERR_INCOMPLETE_LABEL "Label without instruction/directive.\n"
#define ERR_UNKNOWN_INSTRUCTION "Unknown instruction or directive '%s'\n"

#define ERR_OPERAND_FIRST_COMMA "Illegal comma before the first operand.\n"
#define ERR_CONSECUTIVE_COMMAS "Multiple consecutive commas.\n"
#define ERR_MISSING_COMMA "Missing comma between operands.\n"
#define ERR_TOO_MANY_OPRANDS "Too many operands.\n"
#define ERR_TOO_MANY_COMMAS "Too many comma at the end of the line.\n"

#define ERR_SYMBOL_TABLE_MEMORY_ALLOCATION_FAILED "Memory allocation failed for symbol table node.\n"
#define ERR_SYMBOL_TABLE_STRING_MEMORY_ALLOCATION_FAILED "Memory allocation failed for symbol string.\n"

#define ERR_DATA_DIRECTIVE_SYMBOL_ADDITION_FAILED "Addition of data directive to symbol table has failed.\n"
#define ERR_TOO_MANY_QUOTES_IN_STRING ".asciz string has too many quotes."
#define ERR_MISSING_QUOTES "Missing quotes in .asciz definition."
#define ERR_MISSING_CLOSING_QUOTES "Not found missing quote for .asciz."

#define ERR_MISSING_OPERANDS_DATA_DIRECTIVE "Missing operands for data directive."
#define ERR_DATA_NOT_FIT_FOR_TYPE "Data item %l not fitting for %s data type."

#define WARN_LABEL_BEFORE_DIRECTIVE "Label '%s' before '%s' directive is ignored.\n
#define ERR_MISSING_OPRAND_E_DIRECTIVE "Missing operand for %s directive."
#define ERR_TOO_MANY_OPRANDS_E_DIRECTIVE "Too many operands for %s directive."

#define ERR_EXISTING_EXTERN_SYBOL_EXISTS_CODE "Symbol %s declared extern was already defined in this file as code."
#define ERR_EXISTING_EXTERN_SYBOL_EXISTS_DATA "Symbol %s declared extern was already defined in this file as data."

#define ERR_UNKNOWN_INSTRUCTION "Unknown instruction '%s'."
#define ERR_INSTRUCTION_OPRAND_COUNT_LOW "Instruction '%s' got too little arguments expects %d operands, got %d."
#define ERR_INSTRUCTION_OPRAND_COUNT_HIGH "Instruction '%s' got too many arguments expects %d operands, got %d."




#endif
