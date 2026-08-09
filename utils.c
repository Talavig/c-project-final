#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"
#include "messages.h"

/*
 * a lookup table for all available instructions, with all items being Instruction structs
 */
Instruction instructions[] = {
		{"add", R_TYPE, R_INSTRUCTIONS_ARITHMATIC_OPCODES, 3, ADD},
		{"sub", R_TYPE, R_INSTRUCTIONS_ARITHMATIC_OPCODES, 3, SUB},
		{"and", R_TYPE, R_INSTRUCTIONS_ARITHMATIC_OPCODES, 3, AND},
		{"or", R_TYPE, R_INSTRUCTIONS_ARITHMATIC_OPCODES, 3, OR},
		{"nor", R_TYPE, R_INSTRUCTIONS_ARITHMATIC_OPCODES, 3, NOR},

		{"move", R_TYPE, R_INSTRUCTIONS_MEMORY_OPCODES, 2, MOVE},
		{"mvhi", R_TYPE, R_INSTRUCTIONS_MEMORY_OPCODES, 2, MVHI},
		{"mvlo", R_TYPE, R_INSTRUCTIONS_MEMORY_OPCODES, 2, MVLO},

		{"addi", I_TYPE, ADDI, 3, NON_R_FUNCT_VALUE},
		{"subi", I_TYPE, SUBI, 3, NON_R_FUNCT_VALUE},
		{"andi", I_TYPE, ANDI, 3, NON_R_FUNCT_VALUE},
		{"ori", I_TYPE, ORI, 3, NON_R_FUNCT_VALUE},
		{"nori", I_TYPE, NORI, 3, NON_R_FUNCT_VALUE},

		{"bne", I_TYPE, BNE, 3, NON_R_FUNCT_VALUE},
		{"beq", I_TYPE, BEQ, 3, NON_R_FUNCT_VALUE},
		{"blt", I_TYPE, BLT, 3, NON_R_FUNCT_VALUE},
		{"bgt", I_TYPE, BGT, 3, NON_R_FUNCT_VALUE},

		{"lb", I_TYPE, LB, 3, NON_R_FUNCT_VALUE},
		{"sb", I_TYPE, SB, 3, NON_R_FUNCT_VALUE},
		{"lw", I_TYPE, LW, 3, NON_R_FUNCT_VALUE},
		{"sw", I_TYPE, SW, 3, NON_R_FUNCT_VALUE},
		{"lh", I_TYPE, LH, 3, NON_R_FUNCT_VALUE},
		{"sh", I_TYPE, SH, 3, NON_R_FUNCT_VALUE},

		{"jmp", J_TYPE, JMP, 1, NON_R_FUNCT_VALUE},
		{"la", J_TYPE, LA, 1, NON_R_FUNCT_VALUE},
		{"call", J_TYPE, CALL, 1, NON_R_FUNCT_VALUE},
		{"hlt", J_TYPE, HLT, 0, NON_R_FUNCT_VALUE}
};

/*the num of instructions is calculated by the size of the instruction array divided by a single Instruction struct */
const int NUM_INSTRUCTIONS = sizeof(instructions) / sizeof(Instruction);

Instruction * getInstruction(char* instruction_name){
	int i; /* a loop index */

	/* iterate thought the instruction table until getting to an instruction with instruction_name, and returning a pointer to it */
	for (i = 0; i < NUM_INSTRUCTIONS; i++) {
		if (strcmp(instructions[i].name, instruction_name) == 0) {
			return &instructions[i];
		}
	}
	/*otherwise return null */
	return NULL;
}

Boolean parseRegister(const char *str, int *reg_num) {
	char *endp; /* a pointer used in strtol */
	long parsed_val; /* value returned by strtol*/

	/*check if the string is empty or the string does not start with $*/
	if (str == NULL || str[0] != REGISTER_INDICATOR) {
		return FALSE;
	}

	/* parse the register into a number*/
	parsed_val = strtol(str + 1, &endp, 10);

	/* check if the entire string was parsed (aka the entire string was a number)*/
	if (*endp != END_OF_STRING) {
		return FALSE;
	}

	/* check that the parsed number is in the register range of 0-31*/
	if (parsed_val >= 0 && parsed_val < NUM_REGISTERS) {
		*reg_num = (int)parsed_val;
		return TRUE;
	}

	return FALSE;
}


Boolean parseImmediate(const char *str, int *val) {
	char *endp; /* a pointer used in strtol */
	long parsed_val; /* value returned by strtol*/

	/*check if the string is empty*/
	if (str == NULL || *str == END_OF_STRING) {
		return FALSE;
	}

	/* parse the immidiate into a number*/
	parsed_val = strtol(str, &endp, 10);

	/* check if the entire string was parsed (aka the entire string was a number)*/
	if (*endp != END_OF_STRING) {
		return FALSE;
	}

	/* check that the parsed number is in the immidiate range of a 16 bit number*/
	if (parsed_val <= MAX_IMMED_VALUE && parsed_val >= MIN_IMMED_VALUE) {
		*val = (int)parsed_val;
		return TRUE;
	}
	return FALSE;
}


char *skipWhitespaces(char *str) {
	if (str == NULL){
		return NULL;
	}

	/* advance pointer while the current character is a space */
	while (*str != END_OF_STRING && isspace((unsigned char)*str)) {
		str++;
	}
	return str;
}

void cleanLineEnding(char *line) {
	if (line != NULL) {
		/* find the first occurrence of \r or \n and replace it with a \0 */
		line[strcspn(line, "\r\n")] = END_OF_STRING;
	}
}


Boolean getNextToken(char **line, char *token) {
	int i = 0; /* loop index*/
	char *ptr = *line; /* a pointer to the line*/

	/* skip leading spaces */
	while (*ptr != END_OF_STRING && isspace((unsigned char)*ptr)) {
		ptr++;
	}

	/* if end of string or newline is reached, no token is available */
	if (*ptr == END_OF_STRING || *ptr == '\n' || *ptr == '\r') {
		token[0] = END_OF_STRING;
		return FALSE;
	}

	/* extract characters until a whitespace, comma, or line ending is encountered */
	while (*ptr != END_OF_STRING && !isspace((unsigned char)*ptr) && *ptr != ',' && *ptr != '\r' && *ptr != '\n') {
		if (i < MAX_TOKEN_LENGTH - 1) {
			token[i++] = *ptr;
		}
		ptr++;
	}

	/* add a \0 to the end of the string and move the original pointer to after the token*/
	token[i] = END_OF_STRING;
	*line = ptr;
	return TRUE;
}


Status extractOperands(char **line, char operands[][MAX_TOKEN_LENGTH], int *operand_count, int line_counter){
	Boolean expect_comma = FALSE; /* a boolean used to track if a comma needs to appear or not*/

	/* zero out the operand counter and the operand array*/
	*operand_count = 0;
	memset(operands, 0, MAX_OPERANDS_PER_LINE * MAX_TOKEN_LENGTH * sizeof(char));

	/* skip leading spaces */
	while (**line != END_OF_STRING && isspace((unsigned char)**line)) {
		(*line)++;
	}

	/* iterate over the string*/
	while (**line != END_OF_STRING){
		if (**line == ','){
			if (expect_comma){
				/* legal comma found, so now we expect an oprand*/
				(*line)++;
				expect_comma = FALSE;
			}
			else{
				/* we found an illegal comma*/
				if(*operand_count == 0){
					ASM_ERROR(line_counter, (ERR_OPERAND_FIRST_COMMA));
				}
				else{
					ASM_ERROR(line_counter, (ERR_CONSECUTIVE_COMMAS));
				}
				return FAILURE;
			}
		}
		else{
			/* the character is not a comma. if we expect one, throw an error */
			if (expect_comma){
				ASM_ERROR(line_counter, (ERR_MISSING_COMMA));
				return FAILURE;
			}

			/* otherwise, move on to the next operand */
			if (getNextToken(line, operands[*operand_count])){
				(*operand_count)++;
				expect_comma = TRUE;
			}

		}

		/* skip spaces*/
		while (**line != END_OF_STRING && isspace((unsigned char)**line)){
			(*line)++;
		}

	}
	/* if the ends while a comma was the last character read (expect_comma == FALSE), raise an error */
	if (expect_comma == FALSE && *operand_count > 0){
		ASM_ERROR(line_counter, (ERR_TOO_MANY_COMMAS));
		return FAILURE;
	}
	return SUCCESS;
}

char *createFileName(char *base_name, char *extension){
	/* allocate memory for basename + extention size + \0 */
	char *full_name = (char *)malloc(strlen(base_name) + strlen(extension) + 1);

	/* if allocation failed, raise an error. otherwise, copyt the base name and concat the extention */
	if (full_name == NULL) {
		fprintf(stderr, ERR_MEM_ALLOC_FAILED, "file name");
		return NULL;
	}
	strcpy(full_name, base_name);
	strcat(full_name, extension);

	return full_name;
}

Boolean isLineTooLong(const char *line, FILE *file){
	int tmp_character; /*used for iterating in file*/
	int len = strcspn(line, "\r\n"); /* counter characters until first appearence of /r or /n*/

	/* is the line until the new line character more than MAX_SINGLE_LINE_LENGTH long*/
	if (len > MAX_SINGLE_LINE_LENGTH) {
		/* the line we read has more than 80 characters, which means we need to clead the file buffer and return TRUE*/
		if (strchr(line, '\n') == NULL && !feof(file)) {
			while ((tmp_character = fgetc(file)) != '\n' && tmp_character != EOF);
		}
		return TRUE;
	}

	/* edge case where the line is 80 characters long,*/
	if (strchr(line, '\n') == NULL && !feof(file)) {
		/* look at the next character in the file after the end of the file stream we already read*/
		tmp_character = fgetc(file);

		/* if that file is part of a new line*/
		if (tmp_character == '\n' || tmp_character == '\r') {
			/* if its a \r, move the file pointer to the supposed \n */
			if (tmp_character == '\r') {
				tmp_character = fgetc(file);
				/* if we got to the \n and we are at the end of a file, return the extra character to the file buffer*/
				if (tmp_character != '\n' && tmp_character != EOF) {
					ungetc(tmp_character, file);
				}
			}

			/* the line is exactly 80 characters*/
			return FALSE;
		}

		/* the next character is a regular character, and so we flush the file buffer*/
		while (tmp_character != '\n' && tmp_character != EOF) {
			tmp_character = fgetc(file);
		}

		/* the line is too long*/
		return TRUE;
	}
	/* no newline found, but this is the final line so we got to a legal length without a \n*/
	return FALSE;
}

Boolean isEmptyOrComment(char *current_line){
	/* a comment line starts with ; */
	return (*current_line == END_OF_STRING || *current_line == COMMENT) ? TRUE : FALSE;
}

Boolean isLabelDef(char *token){
	/* a label definition must end with a : */
	return (token[strlen(token) - 1] == LABEL_END) ? TRUE : FALSE;
}

Boolean isDataDirective(char* token){
	return (strcmp(token, DB_DIRECTIVE) == 0 || strcmp(token, DW_DIRECTIVE) == 0 || strcmp(token, DH_DIRECTIVE) == 0 || strcmp(token, ASCIZ_DIRECTIVE) == 0) ? TRUE : FALSE;
}

Boolean isExternDirective(char *token){
	return (strcmp(token, EXTERN_DIRECTIVE) == 0) ? TRUE : FALSE;
}

Boolean isEntryDirective(char *token){
	return (strcmp(token, ENTRY_DIRECTIVE) == 0) ? TRUE : FALSE;
}

Boolean isInstruction(char *token) {
	return (getInstruction(token) != NULL) ? TRUE : FALSE;
}


Boolean isReservedWord(char *word){
	int fake_reg; /* used in checking if the word is a register */

	if (word == NULL) {
		return FALSE;
	}

	/* is instruction */
	if (getInstruction(word) != NULL) {
		return TRUE;
	}

	/* is a register*/
	if (parseRegister(word, &fake_reg) == TRUE) {
		return TRUE;
	}

	/* is known directive*/
	if (isDataDirective(word)) {
		return TRUE;
	}

	if (isExternDirective(word) || isEntryDirective(word)) {
		return TRUE;
	}

	/* is a macro start/end*/
	if (strcmp(word, MACRO_START) == 0 || strcmp(word, MACRO_END) == 0) {
		return TRUE;
	}

	/* its none of the above, so its not a reserved word*/
	return FALSE;
}

Boolean isValidLabel(char *str){
	const char *ptr = str; /* a pointer to the string */

	/*is the string empty or null */
	if (str == NULL || *str == END_OF_STRING) {
		return FALSE;
	}

	/* is the label name too long */
	if (strlen(str) > MAX_LABEL_LENGTH) {
		return FALSE;
	}

	/* is the first character not a letter */
	if (!isalpha((unsigned char)*ptr)) {
		return FALSE;
	}

	/*does the label have any non alphanumeric characters */
	while (*ptr != END_OF_STRING) {
		if (!isalnum((unsigned char)*ptr)) {
			return FALSE;
		}
		ptr++;
	}

	/*is the label a reserved word*/
	if (isReservedWord((char *)str)) {
		return FALSE;
	}

	/*it must be a vaild label*/
	return TRUE;
}










