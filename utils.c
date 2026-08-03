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
				(*ptr)++;
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

		while (**ptr != '\0' && isspace((unsigned char)**ptr)){
			(*ptr)++;
		}

	}
	if (expect_comma == FALSE && *operand_count > 0){
		ASM_ERROR(line_counter, (ERR_TOO_MANY_COMMAS));
		return FAILURE;
	}
	return SUCCESS;
}
