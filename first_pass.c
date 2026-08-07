#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "consts.h"
#include "utils.h"
#include "symbol_table.h"
#include "messages.h"

Status handleDataDirective(char **line, char *directive, Boolean has_label, char *label_name,SymbolTable *symbol_table, int *dc, unsigned char *data_image, int line_counter);
Status handleEDirective(char **line, char *directive, Boolean has_label, char *label_name,SymbolTable *symbol_table, unsigned char *data_image, int line_counter);
Status handleInstruction(char **line, char *instruction_name, Boolean has_label, char *label_name, SymbolTable *symbol_table, int *ic, unsigned long* *code_image, int line_counter);
void updateDataSymbolAddresses(SymbolTable *symbol_table, int icf);


Status firstPass(char * file_base_name, SymbolTable* symbol_table, int* ic, int* dc, unsigned char *data_image, unsigned long* code_image){
	FILE *macro_file;
	char* macro_file_name;

	char * current_line[MAX_SINGLE_LINE_LENGTH + 2];
	char *current_line_ptr;
	char token[MAX_TOKEN_LENGTH];
	int token_len;
	char label_name[MAX_LABEL_LENGTH];


	Boolean has_label =  FALSE;
	Status pass_status = SUCCESS;
	int line_counter = 0;


	macro_file_name = createFileName(file_base_name, MACRO_ASSEMBLY_FILE);
	if (macro_file_name == NULL) {
		return FAILURE;
	}

	if ((macro_file = fopen(macro_file_name, "r")) == NULL){
		fprintf(stderr, ERR_CANNOT_OPEN_FILE, macro_file_name);
		free(macro_file_name);
		return FAILURE;
	}


	*ic = IC_INITIAL_VALUE;
	*dc = DC_INITIAL_VALUE;

	while (fgets(current_line, sizeof(current_line), macro_file) != NULL){
		line_counter++;
		has_label = FALSE;
		current_line_ptr = current_line;
		memset(label_name, 0, sizeof(label_name));
		memset(token, 0, sizeof(token));

		if (isLineTooLong(current_line, macro_file)) {
			ASM_ERROR(line_counter, ("Line exceeds maximum length of 80 characters."));
			pass_status = FAILURE;
		}
		else{
			current_line[strcspn(current_line, "\n")] = '\0';
			current_line_ptr = skipWhitespaces(current_line_ptr);
			if (!isEmptyOrComment(current_line_ptr)) {
				if (getNextToken(&current_line_ptr, token)) {
					token_len = strlen(token);
					if (isLabelDef(token)){
						has_label = TRUE;
						token[token_len - 1] = '\0';
						if (!isValidLabel(token)) {
							ASM_ERROR(line_counter, ("Invalid label name '%s'.", token));
							pass_status = FAILURE;
						}
						else{
							strcpy(label_name, token);
							if (!getNextToken(&current_line_ptr, token)){
								ASM_ERROR(line_counter, (ERR_INCOMPLETE_LABEL));
								pass_status = FAILURE;
							}
						}
					}
					if (token[0] != '\0'){
						if(isDataDirective(token) && handleDataDirective(&current_line_ptr, token, has_label, label_name, symbol_table, dc, data_image, line_counter) == FAILURE){
							pass_status = FAILURE;
						}
						else if((isExternDirective(token) || isEntryDirective(token)) && handleEDirective(&current_line_ptr, token, has_label, label_name, symbol_table, data_image, line_counter) == FAILURE){
							pass_status = FAILURE;
						}
						/* entry directives are handled in the second pass */
						else if(isInstruction(token) && handleInstruction(&current_line_ptr, token, has_label, label_name, symbol_table, ic, code_image, line_counter) == FAILURE){
							pass_status = FAILURE;
						}
						else{
							ASM_ERROR(line_counter, (ERR_UNKNOWN_INSTRUCTION, token));
							pass_status = FAILURE;
						}
					}
				}
			}

		}
	}

	fclose(macro_file);
	free(macro_file_name);

	if (pass_status == FAILURE){
		return FAILURE;
	}
	else{
		updateDataSymbolAddresses(*symbol_table, *ic);
		return SUCCESS;
	}
}

Status handleDataDirective(char **line, char *directive, Boolean has_label, char *label_name,SymbolTable *symbol_table, int *dc, unsigned char *data_image, int line_counter){
	SymbolTableEntry data_entry;
	char * first_quote;
	char * last_quote;
	char * line_ptr;
	int tmp_dc = *dc;
	int data_size;
	char operands[MAX_OPERANDS_PER_LINE][MAX_TOKEN_LENGTH];
	int operand_count = 0;
	int i;
	long data_content;
	long data_content_limit_min;
	long data_content_limit_max;
	int byte_index;

	data_size = (strcmp(directive, DB_DIRECTIVE) == 0) ? DB_SIZE : (strcmp(directive, DH_DIRECTIVE) == 0) ? DH_SIZE : DW_SIZE;
	data_content_limit_min = (strcmp(directive, DB_DIRECTIVE) == 0) ? SCHAR_MIN : (strcmp(directive, DH_DIRECTIVE) == 0) ? SHRT_MIN : INT_MIN;
	data_content_limit_max = (strcmp(directive, DB_DIRECTIVE) == 0) ? SCHAR_MAX : (strcmp(directive, DH_DIRECTIVE) == 0) ? SHRT_MAX : INT_MAX;


	if (has_label) {
		if (findSymbol(*symbol_table, label_name) == NULL){
			data_entry.symbol = label_name;
			data_entry.value = *dc;
			data_entry.attributes = DATA;
			if(addEntryToSymbolTable(symbol_table, data_entry) == FAILURE){
				ASM_ERROR(line_counter, (ERR_DATA_DIRECTIVE_SYMBOL_ADDITION_FAILED));
				free(data_entry);
				return FAILURE;
			}
		}
		else{
			ASM_ERROR(line_counter, ("Symbol '%s' already exists.", label_name));
			return FAILURE;
		}
	}

	if (strcmp(directive, ASCIZ_DIRECTIVE) == 0) {
		first_quote = strchr(line, STRING_WRAPPER);
		if(first_quote == NULL){
			ASM_ERROR(line_counter, (ERR_MISSING_QUOTES));
			return FAILURE;
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
		line_ptr = first_quote + 1;
		while(*line_ptr != STRING_WRAPPER){
			data_image[tmp_dc] = *line_ptr;
			line_ptr++;
			tmp_dc++;
		}
		data_image[tmp_dc] = '\0';
		*dc = tmp_dc + 1;
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
			if (data_content > data_content_limit_max || data_content < data_content_limit_min){
				ASM_ERROR(line_counter, (ERR_DATA_NOT_FIT_FOR_TYPE, data_content, directive));
				return FAILURE;
			}

			for (byte_index = 0; byte_index < data_size; byte_index++) {
				data_image[*dc + byte_index] = (data_content >> (byte_index * 8)) & BYTE_MASK;
			}
			*dc += data_size;
		}
	}
	return SUCCESS;
}

Status handleEDirective(char **line, char *directive, Boolean has_label, char *label_name,SymbolTable *symbol_table, unsigned char *data_image, int line_counter){
	char operands[MAX_OPERANDS_PER_LINE][MAX_TOKEN_LENGTH];
	int operand_count = 0;
	SymbolTableNode *existing_symbol;
	SymbolTableEntry new_entry;

	if (has_label) {
		ASM_WARNING(line_counter, (WARN_LABEL_BEFORE_DIRECTIVE, label_name, directive));
	}

	if (extractOperands(line, operands, &operand_count, line_counter) == FAILURE) {
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
			new_entry.symbol = operands[0];
			new_entry.value = 0;
			new_entry.attributes = EXTERNAL;

			if (addEntryToSymbolTable(symbol_table, new_entry) == FAILURE) {
				return FAILURE;
			}
		}

		else{
			if(existing_symbol->symbol_table_entry.attributes & CODE){
				ASM_ERROR(line_counter, (ERR_EXISTING_EXTERN_SYBOL_EXISTS_CODE, existing_symbol->symbol_table_entry.symbol));
				return FAILURE;
			}
			if(existing_symbol->symbol_table_entry.attributes & DATA){
				ASM_ERROR(line_counter, (ERR_EXISTING_EXTERN_SYBOL_EXISTS_DATA, existing_symbol->symbol_table_entry.symbol));
				return FAILURE;
			}
		}
		return SUCCESS;
	}

	else if (strcmp(directive, ENTRY_DIRECTIVE) == 0) {
		return SUCCESS;
	}
	return FAILURE;
}

Status handleInstruction(char **line, char *instruction_name, Boolean has_label, char *label_name, SymbolTable *symbol_table, int *ic, unsigned long* *code_image, int line_counter){
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
	int i;

	if (has_label) {
		if (findSymbol(symbol_table, label_name) == NULL){
			code_entry.symbol = label_name;
			code_entry.value = *ic;
			code_entry.attributes = CODE;
			if(addEntryToSymbolTable(symbol_table, code_entry) == FAILURE){
				ASM_ERROR(line_counter, (ERR_DATA_DIRECTIVE_SYMBOL_ADDITION_FAILED));
				return FAILURE;
			}
		}
		else{
			ASM_ERROR(line_counter, ("Symbol '%s' already defined.", label_name));
			return FAILURE;
		}
	}
	instruction = getInstruction(instruction_name);
	if (instruction == NULL) {
		ASM_ERROR(line_counter, (ERR_UNKNOWN_INSTRUCTION, instruction));
		return FAILURE;
	}

	if (extractOperands(line, operands, &operand_count, line_counter) == FAILURE) {
		return FAILURE;
	}

	if (operand_count < instruction->oprands) {
		ASM_ERROR(line_counter, (ERR_INSTRUCTION_OPRAND_COUNT_LOW, instruction, inst_def->expected_operands, operand_count));
		return FAILURE;
	}

	else if (operand_count > instruction->oprands) {
		ASM_ERROR(line_counter, (ERR_INSTRUCTION_OPRAND_COUNT_HIGH, instruction, inst_def->expected_operands, operand_count));
		return FAILURE;
	}

	machine_code |= (instruction->opcode << OPCODE_SHIFT);


	switch (instruction->type) {
	case R_TYPE:
		switch (instruction->opcode){
		case R_INSTRUCTIONS_ARITHMATIC_OPCODES:
			if (!parseRegister(operands[0], &rs) || !parseRegister(operands[1], &rt) || !parseRegister(operands[2], &rd)){
				ASM_ERROR(line_counter, ("Operands for '%s' must be valid registers ($0-$31).", instruction));
				return FAILURE;
			}
			break;

		case R_INSTRUCTIONS_MEMORY_OPCODES:
			if (!parseRegister(operands[0], &rs) || !parseRegister(operands[1], &rd)) {
				ASM_ERROR(line_counter, ("Operands for '%s' must be valid registers ($0-$31).", instruction));
				return FAILURE;
			}
			break;

		default:
			ASM_ERROR(line_counter, ("Invalid R-type opcode."));
			return FAILURE;
		}

		machine_code |= (rs << RS_SHIFT);
		machine_code |= (rt << RT_SHIFT);
		machine_code |= (rd << RD_SHIFT);
		machine_code |= (instruction->funct << FUNCT_SHIFT);
		break;

		case I_TYPE:
			switch (instruction->opcode){
			case ADDI: case SUBI: case ANDI: case ORI: case NORI: case LB:   case SB:   case LW:   case SW:  case LH:  case SH:
				if (!parseRegister(operands[0], &rs) || !parseImmediate(operands[1], &immed) || !parseRegister(operands[2], &rt)) {
					ASM_ERROR(line_counter, ("Invalid operands for '%s'.", instruction));
					return FAILURE;
				}
				break;

			case BNE: case BEQ: case BLT: case BGT:
				if (!parseRegister(operands[0], &rs) || !parseRegister(operands[1], &rt)) {
					ASM_ERROR(line_counter, ("First two operands for '%s' must be registers.", instruction));
					return FAILURE;
				}
				if (!isValidLabel(operands[2])) {
					ASM_ERROR(line_counter, ("Third operand for '%s' must be a valid label.", instruction));
					return FAILURE;
				}
				break;

			default:
				ASM_ERROR(line_counter, ("Invalid I-type opcode."));
				return FAILURE;
			}

			machine_code |= (rs << RS_SHIFT);
			machine_code |= (rt << RT_SHIFT);
			machine_code |= (immed & IMMED_MASK);
			break;


			case J_TYPE:
				switch (instruction->opcode){
				case HLT:
					break;
				case JMP:
					if (operands[0][0] == REGISTER_INDICATOR) {
						if (!parseRegister(operands[0], &rs)) {
							ASM_ERROR(line_counter, ("Invalid register for jmp."));
							return FAILURE;
						}
						address = rs;
					}
					else{
						if (!isValidLabel(operands[0])) {
							ASM_ERROR(line_counter, ("Invalid label for jmp."));
							return FAILURE;
						}
					}
					break;

				case LA: case CALL:
					if (!isValidLabel(operands[0])) {
						ASM_ERROR(line_counter, ("Operand for '%s' must be a valid label.", instruction));
						return FAILURE;
					}
					break;

				default:
					ASM_ERROR(line_counter, ("Invalid J-type opcode."));
					return FAILURE;
				}
				machine_code |= (reg << REG_JUMP_SHIFT);
				machine_code |= (address & ADDRESS_MASK);
				break;

				default:
					ASM_ERROR(line_counter, ("Unknown instruction type."));
					return FAILURE;
	}

	code_image[(*ic - IC_INITIAL_VALUE) / INSTRUCTION_BYTES_SIZE] = machine_code;

	*ic += INSTRUCTION_BYTES_SIZE;
	return SUCCESS;
}

void updateDataSymbolAddresses(SymbolTable symbol_table, int icf) {
	SymbolTableNode *current = symbol_table;
	if (symbol_table == NULL) return;
	while (current != NULL) {
		if (current->symbol_table_entry.attributes & DATA) {
			current->symbol_table_entry.value += icf;
		}
		current = current->next_entry;
	}
}

