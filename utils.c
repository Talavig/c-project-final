#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"
#include "messages.h"

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

const int NUM_INSTRUCTIONS = sizeof(instructions) / sizeof(Instruction);

Instruction * getInstruction(char* instruction_name){
	int i;
	for (i = 0; i < NUM_INSTRUCTIONS; i++) {
		if (strcmp(instructions[i].name, instruction_name) == 0) {
			return &instructions[i];
		}
	}
	return NULL;
}

Boolean parseRegister(const char *str, int *reg_num) {
	char *endp;
	long parsed_val;

	if (str == NULL || str[0] != REGISTER_INDICATOR) {
		return FALSE;
	}

	parsed_val = strtol(str + 1, &endp, 10);

	if (*endp != '\0') {
		return FALSE;
	}

	if (parsed_val >= 0 && parsed_val < NUM_REGISTERS) {
		*reg_num = (int)parsed_val;
		return TRUE;
	}

	return FALSE;
}


Boolean parseImmediate(const char *str, int *val) {
	printf(str);
	char *endp;
	long parsed_val;

	if (str == NULL || *str == '\0') {
		return FALSE;
	}

	parsed_val = strtol(str, &endp, 10);

	if (*endp != '\0') {
		return FALSE;
	}

	if (parsed_val <= MAX_IMMED_VALUE && parsed_val >= MIN_IMMED_VALUE) {
		*val = (int)parsed_val;
		return TRUE;
	}
	return FALSE;
}


char *skipWhitespaces(char *str) {
	if (str == NULL) return NULL;

	while (*str != '\0' && isspace((unsigned char)*str)) {
		str++;
	}
	return str;
}


Boolean getNextToken(char **line, char *token) {
	int i = 0;
	char *ptr = *line;

	while (*ptr != '\0' && isspace((unsigned char)*ptr)) {
		ptr++;
	}

	if (*ptr == '\0' || *ptr == '\n' || *ptr == '\r') {
		token[0] = '\0';
		return FALSE;
	}

	while (*ptr != '\0' && !isspace((unsigned char)*ptr) && *ptr != ',' && *ptr != '\r' && *ptr != '\n') {
		if (i < MAX_TOKEN_LENGTH - 1) {
			token[i++] = *ptr;
		}
		ptr++;
	}

	token[i] = '\0';
	*line = ptr;
	return TRUE;
}


Status extractOperands(char **line, char operands[][MAX_TOKEN_LENGTH], int *operand_count, int line_counter){
	Boolean expect_comma = FALSE;
	*operand_count = 0;
	memset(operands, 0, MAX_OPERANDS_PER_LINE * MAX_TOKEN_LENGTH * sizeof(char));
	while (**line != '\0' && isspace((unsigned char)**line)) {
		(*line)++;
	}

	while (**line != '\0'){
		if (**line == ','){
			if (expect_comma){
				(*line)++;
				expect_comma = FALSE;
			}
			else{
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
			if (expect_comma){
				ASM_ERROR(line_counter, (ERR_MISSING_COMMA));
				return FAILURE;
			}

			if (getNextToken(line, operands[*operand_count])){
				(*operand_count)++;
				expect_comma = TRUE;
			}

		}

		while (**line != '\0' && isspace((unsigned char)**line)){
			(*line)++;
		}

	}
	if (expect_comma == FALSE && *operand_count > 0){
		ASM_ERROR(line_counter, (ERR_TOO_MANY_COMMAS));
		return FAILURE;
	}
	return SUCCESS;
}

char *createFileName(char *base_name, char *extension){
	char *full_name = (char *)malloc(strlen(base_name) + strlen(extension) + 1);
	if (full_name == NULL) {
		fprintf(stderr, "Error: Memory allocation failed for file name.\n");
		return NULL;
	}
	strcpy(full_name, base_name);
	strcat(full_name, extension);

	return full_name;
}

Boolean isLineTooLong(const char *line, FILE *file){
	int tmp_character;
	if (strchr(line, '\n') == NULL && !feof(file)) {
		while((tmp_character = fgetc(file)) != '\n' && tmp_character != EOF);
		return TRUE;
	}
	return FALSE;
}

Boolean isEmptyOrComment(char *current_line){
	return (*current_line == END_OF_STRING || *current_line == COMMENT) ? TRUE : FALSE;
}

Boolean isLabelDef(char *token){
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
	int fake_reg;

	if (word == NULL) {
		return FALSE;
	}

	if (getInstruction(word) != NULL) {
		return TRUE;
	}

	if (parseRegister(word, &fake_reg) == TRUE) {
		return TRUE;
	}

	if (isDataDirective(word)) {
		return TRUE;
	}

	if (isExternDirective(word) || isEntryDirective(word)) {
		return TRUE;
	}

	if (strcmp(word, MACRO_START) == 0 || strcmp(word, MACRO_END) == 0) {
		return TRUE;
	}
	return FALSE;
}

Boolean isValidLabel(char *str){
	const char *ptr = str;

	if (str == NULL || *str == '\0') {
		return FALSE;
	}

	if (strlen(str) > MAX_LABEL_LENGTH) {
		return FALSE;
	}

	if (!isalpha((unsigned char)*ptr)) {
		return FALSE;
	}

	while (*ptr != '\0') {
		if (!isalnum((unsigned char)*ptr)) {
			return FALSE;
		}
		ptr++;
	}

	if (isReservedWord((char *)str)) {
		return FALSE;
	}

	return TRUE;
}










