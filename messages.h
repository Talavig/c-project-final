#ifndef MESSAGES_H
#define MESSAGES_H

#define ASM_ERROR(line_num, args) \
    do { \
        printf("Error in line %d: ", line_num); \
        printf args; \
        printf("\n"); \
    } while(0)


#define NO_ARGUMENTS_PASSED "Please pass at least one argument for assembler."
#define WRONG_FILE_EXTENTION "You may only pass files with .as to the assembler, please rename your file."

#define ERR_ALLOCATION_FAILED "Internal Error: Memory allocation failed for filenames.\n"
#define ERR_CANNOT_OPEN_FILE "Error: Cannot open assembly file %s\n"
#define ERR_CANNOT_CREATE_FILE "Error: Cannot create macro file %s\n"

#define ERR_LINE_TOO_LONG "Line exceeds maximum length of 80 characters."
#define ERR_EXTRA_CHARS_MACRO_END "Extra characters after 'mcroend'."
#define ERR_INVALID_MACRO_DEF "Invalid macro definition: '%s'."

#define ERR_MACRO_ALLOC_FAILED "Internal Error: Memory allocation failed for new macro.\n"
#define ERR_MACRO_NAME_ALLOC_FAILED "Internal Error: Memory allocation failed for macro name.\n"
#define ERR_MACRO_CONTENT_ALLOC_FAILED "Internal Error: Memory allocation failed for macro content.\n"
#define ERR_MACRO_CONTENT_REALLOC_FAILED "Internal Error: Memory reallocation failed for macro content.\n"


#define ERR_INCOMPLETE_LABEL "Error in line %d: Label without instruction/directive.\n"
#define ERR_UNKNOWN_INSTRUCTION "Error in line %d: Unknown instruction or directive '%s'\n"

#define ERR_OPERAND_FIRST_COMMA "Error: Illegal comma before the first operand.\n"
#define ERR_CONSECUTIVE_COMMAS "Error: Multiple consecutive commas.\n"
#define ERR_MISSING_COMMA "Error in line %d: Missing comma between operands.\n"
#define ERR_TOO_MANY_OPRANDS "Error in line %d: Too many operands.\n"
#define ERR_TOO_MANY_COMMAS "Error in line %d: Too many comma at the end of the line.\n"

#define ERR_SYMBOL_TABLE_MEMORY_ALLOCATION_FAILED "Error: Memory allocation failed for symbol table node.\n"
#define ERR_SYMBOL_TABLE_STRING_MEMORY_ALLOCATION_FAILED "Error: Memory allocation failed for symbol string.\n"

#endif
