#ifndef UTILS_H
#define UTILS_H

#include "consts.h"

/*
 * advance a string pointer past any leading whitespace characters
 * str: The original string pointer
 *
 * return a pointer to the first non-whitespace character in the string
 */
char *skipWhitespaces(char *str);

/*
 * remove line ending characters ('\n', '\r') from the end of a string
 * line: The string to be cleaned
 */
void cleanLineEnding(char *line);

/*
 * extract the next token (word) from a string, separated by spaces or commas.
 * update the source pointer to point to the character immediately following the extracted token
 * src: a pointer to the source string pointer
 * dest: The buffer where the extracted token will be stored
 *
 * return TRUE if a token was successfully extracted, FALSE if the end of the string was reached
 */
Boolean getNextToken(char **src, char *dest);

/*
 * extracts all operands from a line, validating proper comma placement.
 * ensure commas separate operands correctly, without leading, trailing, or consecutive commas.
 * line: pointer to the current position in the line buffer
 * operands: a 2D array where the extracted operands will be stored
 * operand_count: pointer to an integer that will hold the number of operands extracted
 * line_counter: The current line number for logging
 *
 * return SUCCESS if operands were extracted with valid syntax, FAILURE if a syntax error was found
 */
Status extractOperands(char **line, char operands[][MAX_TOKEN_LENGTH], int *operand_count, int line_counter);

/*
 * concatenate a base name and an extension to create a full file name
 * base_name: the base name of the file (e.g., "my_prog")
 * extension: the file extension (e.g., ".as")
 *
 * return a dynamically allocated string containing the full file name, or NULL an error was encountered
 */
char *createFileName(const char *base_name, const char *extension);

/*
 * check if a line exceeds the maximum allowed length as shown in manual, including \r\n new lines
 * safely handle edge cases such as reaching EOF without a newline character, and clear the remainder of the long line from the input buffer
 * line: the line buffer that was read
 * file: the file pointer being read from
 *
 * return TRUE if the line is too long, FALSE otherwise
 */
Boolean isLineTooLong(const char *line, FILE *file);

/*
 * check if a line is empty or is a comment line
 * current_line: the line to check
 *
 * return TRUE if empty or a comment, FALSE otherwise
 */
Boolean isEmptyOrComment(const char *current_line);

/*
 * check if a token represents a label definition (ends with a colon)
 * token: the token to check
 *
 * return TRUE if it's a label definition, FALSE otherwise
 */
Boolean isLabelDef(const char *token);

/*
 * check if a token is a data storage directive (.db, .dw, .dh, .asciz)
 * token: the token to check
 *
 * @return TRUE if it's a data directive, FALSE otherwise
 */
Boolean isDataDirective(const char *token);

/*
 * check if a token is an extern directive (.extern)
 * token: the token to check
 *
 * return TRUE if it's an extern directive, FALSE otherwise
 */
Boolean isExternDirective(const char *token);

/*
 * check if a token is an entry directive (.entry)
 * token: the token to check
 *
 * return TRUE if it's an entry directive, FALSE otherwise
 */
Boolean isEntryDirective(const char *token);

/*
 * checks if a given word is a reserved word in the assembly language (instructions, registers, directives, and macro keywords)
 * word: The word to check
 *
 * return TRUE if the word is reserved, FALSE otherwise
 */
Boolean isReservedWord(const char *word);

/*
 * validate if a given string is a legal label name:
 * check for maximum length, starting with an alphabetic character, containing only
 * alphanumeric characters, and not being a reserved word
 * str: The string to validate.
 *
 * return TRUE if the string is a valid label, FALSE otherwise
 */
Boolean isValidLabel(const char *str);

/*
 * parse a string to check if it represents a valid register, as definjed by the manual
 * str: The string to parse
 * reg_num: pointer to an integer where the parsed register number will be stored
 *
 * return TRUE if parsing was successful and the register is valid (0-31), FALSE otherwise
 */
Boolean parseRegister(const char *str, int *reg_num);

/*
 * parse a string into an immediate integer value. validate that the parsed value fits within the defined limits (16-bit signed integer limits)
 * str: The string to parse
 * val: Pointer to an integer where the parsed value will be stored
 *
 * return TRUE if parsing was successful and the value is within bounds, FALSE otherwise
 */
Boolean parseImmediate(const char *str, int *val);

/*
 * retrieve the instruction data structure for a given instruction name
 * instruction_name: The name of the instruction
 *
 * return a pointer to the Instruction structure if found, NULL if the instruction does not exist
 */
Instruction *getInstruction(const char *instruction_name);

/*
 * check if a token is a valid instruction name
 * name: The token to check
 *
 * return TRUE if the token is an instruction, FALSE otherwise
 */
Boolean isInstruction(const char *name);

#endif
