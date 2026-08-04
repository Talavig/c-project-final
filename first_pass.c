
#include <limits.h>

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
				else if(isDataDirective(token) && handleDataDirective(&line, token, has_label, label_name, &symbol_table, dc, data_image, line_counter) == FAILURE){
					pass_status = FAILURE;
				}
				else if((isExternDirective(token) || isEntryDirective(token)) && handleEDirective(&line, token, has_label, label_name, &symbol_table, dc, data_image, line_counter) == FAILURE){
					pass_status = FAILURE;
				}
				/* entry directives are handled in the second pass */
				else if(isInstruction(token) && handleInstruction(&line, token, has_label, label_name, &symbol_table, ic, code_image, line_counter) == FAILURE)){
					pass_status = FAILURE;
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
	char ** first_quote;
	char ** last_quote;
	char ** line_ptr;
	int tmp_dc = *dc;
	int data_size;
	char operands[MAX_OPERANDS_PER_LINE][MAX_TOKEN_LENGTH];
	int operand_count;
	int i;
	long data_content;
	long data_content_limit_min;
	long data_content_limit_max;

	int operand_count = 0;
	data_size = (strcmp(directive, DB_DIRECTIVE) == 0) ? DB_SIZE : (strcmp(directive, DH_DIRECTIVE) == 0) ? DH_SIZE : DW_SIZE;
	data_content_limit_min = (strcmp(directive, DB_DIRECTIVE) == 0) ? SCHAR_MIN : (strcmp(directive, DH_DIRECTIVE) == 0) ? SHRT_MIN : INT_MIN;
	data_content_limit_max = (strcmp(directive, DB_DIRECTIVE) == 0) ? SCHAR_MAX : (strcmp(directive, DH_DIRECTIVE) == 0) ? SHRT_MAX : INT_MAX;


	if (has_label) {
		if (findSymbol(SymbolTable symbol_table, label_name) == NULL){
			data_entry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
			data_entry.symbol = (char *)malloc(strlen(label_name) + 1);
			strcpy(data_entry.symbol, label_name);
			data_entry.value = *dc;
			data_entry.attributes = DATA;
			if(addEntryToSymbolTable(symbol_table, data_entry) == FAILURE){
				ASM_ERROR(line_counter, (ERR_DATA_DIRECTIVE_SYMBOL_ADDITION_FAILED));
				free(data_entry);
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

		for(i = 0; i < operand_count; i++){
			data_content = atol(operands[i]);
			if (data_content >= data_content_limit_max || data_content <= data_content_limit_min){
				ASM_ERROR(line_counter, (ERR_DATA_NOT_FIT_FOR_TYPE, data_content, directive));
				return FAILURE
			}

			if (data_size == DB_SIZE) {
				data_image[*dc] = value & BYTE_MASK;
			}
			if (data_size == DH_SIZE) {
				data_image[*dc + 1] = (value >> 8) & BYTE_MASK;
			}
			if (data_size == DW_SIZE){
				data_image[*dc + 2] = (value >> 16) & BYTE_MASK;
				data_image[*dc + 3] = (value >> 24) & BYTE_MASK;
			}
			*dc += data_size
		}
	}
	return SUCCESS;
}

Status handleEDirective(char **line, char *directive, Boolean has_label, char *label_name,SymbolTable *symbol_table, unsigned char[] *data_image, int line_counter){
	char operands[MAX_OPERANDS_PER_LINE][MAX_TOKEN_LENGTH];
	int operand_count;
	SymbolTableNode *existing_symbol;
	SymbolTableEntry new_entry;

	if (has_label) {
		ASM_WARNING(line_counter, (WARN_LABEL_BEFORE_DIRECTIVE, label_name, directive));
	}

	if (extractOperands(ptr, operands, &operand_count, line_counter) == FAILURE) {
		return FAILURE;
	}

	if (operand_count == 0) {
		ASM_ERROR(line_counter, (ERR_MISSING_OPRAND_E_DIRECTIVE, directive));
		return FAILURE;
	}
	else if (operand_count > 1) {
		ASM_ERROR(line_counter, (ERR_TOO_MANY_OPRANDS_E_DIRECTIVE, directive));
		return FAILURE;
	}

	if (strcmp(directive, EXTERN_DIRECTIVE) == 0){
		existing_symbol = findSymbol(*symbol_table, operands[0]);
		if(existing_symbol == NULL){
			new_entry.symbol = (char *)malloc(strlen(operands[0]) + 1);
			if (new_entry.symbol == NULL) {
				ASM_ERROR(line_counter, (ERR_SYMBOL_TABLE_STRING_MEMORY_ALLOCATION_FAILED));
				return FAILURE;
			}
			strcpy(new_entry.symbol, operands[0]);
			new_entry.value = 0;
			new_entry.attributes = EXTERNAL;

			if (addEntryToSymbolTable(symbol_table, new_entry) == FAILURE) {
				free(new_entry.symbol);
				return FAILURE;
			}
		}

		else{
			existing_symbol = findSymbol(*symbol_table, operands[0]);
			if (existing_symbol != NULL) {
				if(existing_symbol->entry.attributes & ATTR_CODE){
					ASM_ERROR(line_counter, (ERR_EXISTING_EXTERN_SYBOL_EXISTS_CODE, existing_symbol->entry.name));
					return FAILURE;
				}
				if(existing_symbol->entry.attributes & ATTR_DATA){
					ASM_ERROR(line_counter, (ERR_EXISTING_EXTERN_SYBOL_EXISTS_DATA, existing_symbol->entry.name));
					return FAILURE;
				}
			}
		}
		return SUCCESS;
	}

	else if (strcmp(directive, ENTRY_DIRECTIVE) == 0) {
		return SUCCESS;
	}
}

Status handleInstruction(char **line, char *instruction_name, Boolean has_label, char *label_name, SymbolTable *symbol_table, int *ic, unsigned int[]* *code_image, int line_counter){
	SymbolTableEntry code_entry;
	Instruction * instruction;
	char operands[MAX_OPERANDS_PER_LINE][MAX_TOKEN_LENGTH];
	int operand_count = 0;
	unsigned long machine_code = 0;
	int rs = 0, rt = 0, rd = 0;
	int immed = 0;
	int funct = 0;
	int reg = 0;
	long address = 0;

	if (has_label) {
		if (findSymbol(SymbolTable symbol_table, label_name) == NULL){
			code_entry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
			code_entry.symbol = (char *)malloc(strlen(label_name) + 1);
			strcpy(code_entry.symbol, label_name);
			code_entry.value = *ic;
			code_entry.attributes = CODE;
			if(addEntryToSymbolTable(symbol_table, code_entry) == FAILURE){
				ASM_ERROR(line_counter, (ERR_DATA_DIRECTIVE_SYMBOL_ADDITION_FAILED));
				free(code_entry);
				return FAILURE;
			}

		}
	}
	instruction = getInstruction(instruction);
	if (instruction == NULL) {
		ASM_ERROR(line_counter, (ERR_UNKNOWN_INSTRUCTION, instruction));
		return FAILURE;
	}

	if (extractOperands(ptr, operands, &operand_count, line_counter) == FAILURE) {
		return FAILURE;
	}

	if (operand_count < inst_def->expected_operands) {
		ASM_ERROR(line_counter, (ERR_INSTRUCTION_OPRAND_COUNT_LOW, instruction, inst_def->expected_operands, operand_count));
		return FAILURE;
	}

	else if (operand_count > inst_def->expected_operands) {
		ASM_ERROR(line_counter, (ERR_INSTRUCTION_OPRAND_COUNT_HIGH, instruction, inst_def->expected_operands, operand_count));
		return FAILURE;
	}

	machine_code |= (instruction->opcode << OPCODE_SHIFT);

	if (instruction->type == R_TYPE) {

	}

	else if (inst_def->type == I_TYPE) {

	}

	else if (inst_def->type == J_TYPE) {

	}

	*ic += INSTRUCTION_BYTES_SIZE;
	return SUCCESS;
}
