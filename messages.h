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

#endif
