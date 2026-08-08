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

/*
 * system & file messages
 */
#define NO_ARGUMENTS_PASSED "Please pass at least one argument for assembler.\n"
#define ERR_INVAILD_FILE_EXTENTION "File '%s' does not have a valid '%s' extension. Skipping to next file.\n"
#define WRONG_FILE_EXTENTION "You may only pass files with .as to the assembler, please rename your file.\n"
#define ERR_MEM_ALLOC_FAILED "Internal Error: Memory allocation failed for %s.\n"
#define ERR_FILE_OPERATION_FAILED "Cannot %s %s file: %s\n"

/*
 * syntax  errors
 */
#define ERR_LINE_TOO_LONG "Line exceeds maximum length of 80 characters.\n"
#define ERR_UNKNOWN_INST_OR_DIR "Unknown instruction or directive '%s'.\n"
#define ERR_INVALID_LABEL_NAME "Invalid label name '%s'.\n"
#define ERR_INCOMPLETE_LABEL "Label without instruction/directive.\n"
#define ERR_SYMBOL_ALREADY_DEFINED "Symbol '%s' already defined.\n"
#define ERR_OPERAND_FIRST_COMMA "Illegal comma before the first operand.\n"
#define ERR_CONSECUTIVE_COMMAS "Multiple consecutive commas.\n"
#define ERR_MISSING_COMMA "Missing comma between operands.\n"
#define ERR_TOO_MANY_OPRANDS "Too many operands.\n"
#define ERR_TOO_MANY_COMMAS "Too many commas at the end of the line.\n"

/*
 * preprocessor macro errors
 */
#define ERR_INVALID_MACRO_DEF "Invalid macro definition: '%s'.\n"
#define ERR_NO_MACRO_NAME "Missing macro name after macro start directive.\n"
#define ERR_MACRO_NAME_RESERVED_WORD "Macro name cannot be a reserved word.\n"
#define ERR_EXTRA_TEXT_AFTER_MACRO_NAME "Extraneous text after macro name.\n"
#define ERR_EXTRA_CHARS_MACRO_END "Extra characters after 'mcroend'.\n"

/*
 * first pass data directive errors
 */
#define ERR_DATA_DIRECTIVE_SYMBOL_ADDITION_FAILED "Addition of data directive to symbol table has failed.\n"
#define ERR_MISSING_OPERANDS_DATA_DIRECTIVE "Missing operands for data directive.\n"
#define ERR_DATA_NOT_FIT_FOR_TYPE "Data item %ld not fitting for %s data type.\n"
#define ERR_MISSING_QUOTES "Missing quotes in .asciz definition.\n"
#define ERR_MISSING_CLOSING_QUOTES "Not found missing quote for .asciz.\n"
#define ERR_TOO_MANY_QUOTES_IN_STRING ".asciz string has too many quotes.\n"

/*
 * first pass E directive errors
 */
#define WARN_LABEL_BEFORE_DIRECTIVE "Label '%s' before '%s' directive is ignored.\n"
#define ERR_MISSING_OPRAND_E_DIRECTIVE "Missing operand for %s directive.\n"
#define ERR_TOO_MANY_OPRANDS_E_DIRECTIVE "Too many operands for %s directive.\n"
#define ERR_EXISTING_EXTERN_SYBOL_EXISTS "Symbol %s declared extern was already defined in this file as %s.\n"

/*
 * first pass instruction errors
 */
#define ERR_UNKNOWN_INSTRUCTION_NAME "Unknown instruction '%s'.\n"
#define ERR_UNKNOWN_INSTRUCTION_TYPE "Unknown instruction type.\n"
#define INVALID_R_OPCODE "Invalid R-type opcode.\n"
#define INVALID_I_OPCODE "Invalid I-type opcode.\n"
#define INVALID_J_OPCODE "Invalid J-type opcode.\n"

#define ERR_INSTRUCTION_OPRAND_COUNT_LOW "Instruction '%s' expects %d operands, got %d.\n"
#define ERR_INSTRUCTION_OPRAND_COUNT_HIGH "Instruction '%s' expects %d operands, got %d.\n"
#define ERR_INVALID_OPRANDS "Invalid operands for '%s'.\n"
#define ERR_OPERAND_MUST_BE_VALID_REGISTER "Operands for '%s' must be valid registers ($0-$31).\n"
#define ERR_OPERANDS_ARE_NOT_REGISTERS "First two operands for '%s' must be registers.\n"
#define ERR_OPERAND_MUST_BE_VALID_LABEL "Operand for '%s' must be a valid label.\n"
#define ERR_OPERAND_IS_NOT_LABEL "Third operand for '%s' must be a valid label.\n"
#define ERR_INVALID_REGISTER_JMP "Invalid register for jmp.\n"
#define ERR_INVALID_LABEL_JMP "Invalid label for jmp.\n"

/*
 * second pass errors
 */
#define ERR_MISSING_LABEL_AFTER_ENTRY "Missing label name after .entry directive.\n"
#define ERR_ENTRY_LABEL_NOT_DEFINED "Entry label '%s' is not defined in the source file.\n"
#define ERR_SYMBOL_BOTH_ENTRY_AND_EXTERN "Symbol '%s' cannot be both ENTRY and EXTERNAL.\n"
#define ERR_UNDEFINED_LABEL_IN_BRANCH "Undefined label '%s' used in branch instruction.\n"
#define ERR_UNDEFINED_LABEL_J_TYPE "Undefined label '%s' used in J-type instruction.\n"
#define ERR_BRANCH_TO_EXTERN "Cannot branch to external symbol '%s'.\n"
#define ERR_FAILED_TO_RECORD_EXTERN "Failed to record external symbol usage.\n"

/*
 * assembling process info
 */

#define INFO_ASSEMBLING "Assembling %s...\n"
#define INFO_SUCCESSULLY_COMPILED "Successfully assembled %s!\n"
#define INFO_FINISHED_ASSEMBLING "Finished assembling everything!\n\n"
#define PREPROCESSING_ERRORS_FOUND "Errors found during the preprocess of %s. Skipping to next file.\n\n"
#define FIRST_PASS_ERRORS_FOUND "Errors found during the first pass of %s. Skipping to next file.\n\n"
#define SECOND_PASS_ERRORS_FOUND "Errors found during the second pass of %s. Skipping to next file.\n\n"
#define ERR_GENERATION_OUTPUT_FILES_FAILED "Failed to generate output files for %s.\n"


#endif
