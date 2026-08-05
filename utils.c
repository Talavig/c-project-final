#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "consts.h"



char *skipWhitespaces(char *str) {
	if (str == NULL) return NULL;

	while (*str != '\0' && isspace((unsigned char)*str)) {
		str++;
	}
	return str;
}


Boolean getNextToken(char **src, char *dest) {
	int i = 0;

	*src = skipWhitespaces(*src);
	if (**src == '\0') {
		dest[0] = '\0';
		return FALSE;
	}

	while (**src != '\0' && !isspace((unsigned char)**src) && **src != ','){
		dest[i] = **src;
		(*src)++;
		i++;
	}

	dest[i] = '\0';
	return TRUE;
}


Status extractOperands(char **line, char operands[][MAX_TOKEN_LENGTH], int *operand_count, int line_counter){
	Boolean expect_comma = FALSE;
	*operand_count = 0;
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

			if (getNextToken(ptr, operands[*operand_count])){
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


Instruction * getInstruction(char* instruction_name){
	int i;
	for (i = 0; i < NUM_INSTRUCTIONS; i++) {
		if (strcmp(instructions[i].name, name) == 0) {
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

	if (parsed_val > 0 && parsed_val < NUM_REGISTERS) {
		*reg_num = (int)parsed_val;
		return TRUE;
	}

	return FALSE;
}


Boolean parseImmediate(const char *str, int *val) {
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









