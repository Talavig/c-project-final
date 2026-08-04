
#include "consts.h"
#include "utils.h"
#include "symbol_table.h"


Boolean isValidLabel(char * str);


Status firstPass(char * file_base_name, SymbolTable* symbol_table, int* ic, int* dc, unsigned char[] *data_image, unsigned int[]* code_image){
	char * current_line[MAX_SINGLE_LINE_LENGTH + 2];
	int line_counter = 0;
	FILE *macro_file;
	char* macro_file_name;
	Status pass_status = SUCCESS;
	Boolean has_label =  FALSE;
	char label_name[MAX_LABEL_LENGTH];
	char *current_line_ptr;
	char token[MAX_TOKEN_LENGTH];
	int token_len;
	char operands[MAX_OPERANDS_PER_LINE][MAX_TOKEN_LENGTH];
	int operand_count = 0;


	macro_file_name = (char *)malloc(strlen(file_base_name) + 4);
	strcpy(macro_file_name, file_base_name);
	strcat(macro_file_name, INPUT_ASSEMBLY_FILE);

	if ((macro_file = fopen(macro_file_name, "r")) == NULL){
		fprintf(stderr, ERR_CANNOT_OPEN_FILE, macro_file_name);
		free(macro_file_name);
		return FAILURE
	}


	*ic = IC_INITIAL_VALUE;
	*dc = DC_INITIAL_VALUE;

	while (fgets(current_line, sizeof(line), file) != NULL){
		has_label = FALSE;
		current_line_ptr = current_line;
		memset(label_name, 0, sizeof(label_name));
		memset(token, 0, sizeof(token));
		line_counter++;

		current_line[strcspn(current_line, "\n")] = '\0';
		current_line_ptr = skipWhitespaces(current_line_ptr);
		if (!isEmptyOrComment(current_line_ptr)) {
			if (getNextToken(&current_line_ptr, token)) {
				token_len = strlen(token);
				if (isLabelDef(token)){
					has_label = TRUE;
					token[strlen(token) - 1] = '\0';
					strcpy(label_name, token);

					if (!getNextToken(&ptr, token)){
						ASM_ERROR(line_counter, (ERR_INCOMPLETE_LABEL));
						pass_status = FAILURE;
					}
				}
				else if(isDataDirective(token) && handleDataDirective(char **line, char *directive, Boolean has_label, char *label_name,SymbolTable *symbol_table, int *dc, DataImage *data_image, int line_counter) == FAILURE){
					pass_status = FAILURE;
				}
				else if(isExternDirective(token)){
					handleExternDirective();
				}
				/* entry directives are handled in the second pass */
				else if(isInstruction(token)){
					if (has_label){

					}
					*ic += INSTRUCTION_BYTES_SIZE;
				}
				else{
					ASM_ERROR(line_counter, (ERR_UNKNOWN_INSTRUCTION, token));
					Status pass_status = FAILURE;
				}

			}
		}
	}

	fclose(macro_file);
	if (pass_status == FAILURE){
		return FAILURE;
	}
	else{

		return SUCCESS;
	}
}

Boolean isEmptyOrComment(char *current_line){
	return *(current_line == END_OF_STRING || *current_line == COMMENT) ? TRUE : FALSE;
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

Boolean isInstruction(char* token){
	int i;
	for (i = 0; i < NUM_INSTRUCTIONS; i++){
		if (strcmp(instructions[i]->name, name) == 0){
			return TRUE;
		}
	}
	return FALSE;
}

Status handleDataDirective(char **line, char *directive, Boolean has_label, char *label_name,SymbolTable *symbol_table, int *dc, unsigned char[] *data_image, int line_counter){
	SymbolTableEntry data_entry;
	data_entry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
	char ** first_quote;
	char ** last_quote;
	char ** line_ptr;
	int tmp_dc = *dc;
	int data_size;
	char operands[MAX_OPERANDS_PER_LINE][MAX_TOKEN_LENGTH];

	int operand_count = 0;
	data_size = (strcmp(directive, DB_DIRECTIVE) == 0) ? DB_SIZE : (strcmp(directive, DW_DIRECTIVE) == 0) ? DW_SIZE : DH_SIZE;


	if (has_label) {
		if (findSymbol(SymbolTable symbol_table, label_name) == NULL){
			data_entry.symbol = (char *)malloc(strlen(label_name) + 1);
			strcpy(data_entry.symbol, label_name);
			data_entry.value = *dc;
			data_entry.attributes = DATA;
			if(addEntryToSymbolTable(symbol_table, data_entry) == FAILURE){
				ASM_ERROR(line_counter, (ERR_DATA_DIRECTIVE_SYMBOL_ADDITION_FAILED));
				return FAILURE;
			}

		}
	}

	if (strcmp(directive, ASCIZ_DIRECTIVE) == 0) {
		first_quote = strchr(line, STRING_WRAPPER);
		if(first_quote == NULL){
			ASM_ERROR(line_counter, (ERR_MISSING_QUOTES));
		}
		line_ptr = first_quote;
		while(*line_ptr != '\0' && *line_ptr != STRING_WRAPPER){
			(*line_ptr)++;
		}
		last_quote = strrchr(line, STRING_WRAPPER);
		if(last_quote == first_quote){
			ASM_ERROR(line_counter, (ERR_MISSING_CLOSING_QUOTES));
		}
		if (line_ptr != last_quote){
			ASM_ERROR(line_counter, (ERR_TOO_MANY_QUOTES_IN_STRING));
		}
		line_ptr = first_quote;
		while(*line_ptr != '\0' && *line_ptr != STRING_WRAPPER){
			data_image[tmp_dc] = *line_ptr;
			(*line_ptr)++;
			tmp_dc++;
		}
		data_image[tmp_dc] = '\0';
		*dc += tmp_dc - *dc;
	}
	else{
		if (extractOperands(line, operands, &operand_count, line_counter) == FAILURE){
			return FAILURE;
		}
		if (operand_count == 0) {
			ASM_ERROR(line_counter, (ERR_MISSING_OPERANDS_DATA_DIRECTIVE));
			return FAILURE;
		}
	}
	return SUCCESS;
}

Status handleExternDirective(){

}

Status handleInstruction(){

}
