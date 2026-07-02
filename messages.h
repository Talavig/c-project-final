#define ASM_ERROR(line_num, args) \
    do { \
        printf("Error in line %d: ", line_num); \
        printf args; \
        printf("\n"); \
    } while(0)


#define NO_ARGUMENTS_PASSED "Please pass at least one argument for assembler."
#define WRONG_FILE_EXTENTION "You may only pass files with .as to the assembler, please rename your file."
