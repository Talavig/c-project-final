#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "consts.h"
#include "utils.h"
#include "symbol_table.h"
#include "extern_table.h"
#include "messages.h"
#include "second_pass.h"

Status handleEntryDirective(char **line, SymbolTable *symbol_table, int line_counter);
Status encodeCodeTraversalInstructions(char **line, char *instruction_name, SymbolTable *symbol_table, int ic, unsigned long *code_image, ExternTable *extern_table, int line_counter);

Status secondPass(char *file_base_name, SymbolTable *symbol_table, unsigned long *code_image, ExternTable *extern_table) {
	FILE *macro_file;
	char *macro_file_name;

	char current_line[MAX_SINGLE_LINE_LENGTH + 2];
	char *current_line_ptr;
	char token[MAX_TOKEN_LENGTH];

	int second_pass_ic = IC_INITIAL_VALUE;

	Status pass_status = SUCCESS;
	int line_counter = 0;

	macro_file_name = createFileName(file_base_name, MACRO_ASSEMBLY_FILE);
	if ((macro_file = fopen(macro_file_name, "r")) == NULL){
		fprintf(stderr, ERR_CANNOT_OPEN_FILE, macro_file_name);
		free(macro_file_name);
		return FAILURE;
	}

	while (fgets(current_line, sizeof(current_line), macro_file) != NULL) {
		line_counter++;
		current_line_ptr = current_line;


		if (isLineTooLong(current_line, macro_file)) {
			ASM_ERROR(line_counter, (ERR_LINE_TOO_LONG));
			pass_status = FAILURE;
		}
		else{
			current_line[strcspn(current_line, "\n")] = '\0';
			current_line_ptr = skipWhitespaces(current_line_ptr);

			if (!isEmptyOrComment(current_line_ptr)) {
				if (getNextToken(&current_line_ptr, token)) {
					if (isLabelDef(token)) {
						if (!getNextToken(&current_line_ptr, token)) {
							token[0] = '\0';
						}
					}

					if (token[0] != '\0') {
						if (isEntryDirective(token)) {
							if (handleEntryDirective(&current_line_ptr, symbol_table, line_counter) == FAILURE) {
								pass_status = FAILURE;
							}
						}
						else if (isInstruction(token) == FALSE) {
							if (encodeCodeTraversalInstructions(&current_line_ptr, token, symbol_table, second_pass_ic, code_image, extern_table, line_counter) == FAILURE) {
								pass_status = FAILURE;
							}
							second_pass_ic += INSTRUCTION_BYTES_SIZE;
						}
						else {
							ASM_ERROR(line_counter, (ERR_UNKNOWN_INSTRUCTION_OR_DIRECTIVE, token));
							pass_status = FAILURE;
						}
					}
				}
			}
		}
	}
	fclose(macro_file);
	return pass_status;
}

Status handleEntryDirective(char **line, SymbolTable *symbol_table, int line_counter) {
	char label_name[MAX_TOKEN_LENGTH];
	SymbolTableNode *symbol_node;

	if (!getNextToken(line, label_name)) {
		ASM_ERROR(line_counter, (ERR_MISSING_LABEL_AFTER_ENTRY));
		return FAILURE;
	}

	symbol_node = findSymbol(*symbol_table, label_name);
	if (symbol_node == NULL) {
		ASM_ERROR(line_counter, (ERR_ENTRY_LABEL_NOT_DEFINED, label_name));
		return FAILURE;
	}

	if (symbol_node->symbol_table_entry.attributes & EXTERNAL) {
		ASM_ERROR(line_counter, (ERR_SYMBOL_BOTH_ENTRY_AND_EXTERN, label_name));
		return FAILURE;
	}

	symbol_node->symbol_table_entry.attributes |= ENTRY;

	return SUCCESS;
}

Status encodeCodeTraversalInstructions(char **line, char *instruction_name, SymbolTable *symbol_table, int ic, unsigned long *code_image, ExternTable *extern_table, int line_counter){
	Instruction *instruction;
	char operands[MAX_OPERANDS_PER_LINE][MAX_TOKEN_LENGTH];
	int operand_count = 0;
	SymbolTableNode *symbol_node;
	ExternTableEntry extern_entry;

	char *label_name;
	long target_address;
	int offset;

	int code_index = (ic - IC_INITIAL_VALUE) / INSTRUCTION_BYTES_SIZE;
	unsigned long *machine_code = &code_image[code_index];

	instruction = getInstruction(instruction_name);
	if (instruction == NULL) {
		ASM_ERROR(line_counter, (ERR_UNKNOWN_INSTRUCTION, instruction_name));
		return FAILURE;
	}

	if (extractOperands(line, operands, &operand_count, line_counter) == FAILURE) {
		return FAILURE;
	}

	switch (instruction->type) {
	case R_TYPE:
		break;
	case I_TYPE:
		switch (instruction->opcode) {
		case BNE: case BEQ: case BLT: case BGT:
			label_name = operands[2];
			symbol_node = findSymbol(*symbol_table, label_name);
			if (symbol_node == NULL) {
				ASM_ERROR(line_counter, (ERR_UNDEFINED_LABEL_IN_BRANCH, label_name));
				return FAILURE;
			}

			if (symbol_node->symbol_table_entry.attributes & EXTERNAL) {
				ASM_ERROR(line_counter, (ERR_BRANCH_TO_EXTERN, label_name));
				return FAILURE;
			}

			offset = symbol_node->symbol_table_entry.value - ic;
			*machine_code |= ((unsigned long)offset & IMMED_MASK);
			break;
		default:
			break;
		}
		break;
	case J_TYPE:
		switch (instruction->opcode) {
		case JMP:
			if (operands[0][0] == REGISTER_INDICATOR) {
				break;
			}
		case LA: case CALL:
			label_name = operands[0];
			symbol_node = findSymbol(*symbol_table, label_name);
			if (symbol_node == NULL) {
				ASM_ERROR(line_counter, (ERR_UNDEFINED_LABEL_J_TYPE, label_name));
				return FAILURE;
			}

			if (symbol_node->symbol_table_entry.attributes & EXTERNAL) {
				strcpy(extern_entry.symbol, label_name);
				extern_entry.address = ic;

				if (addEntryToExternTable(extern_table, extern_entry) == FAILURE) {
					ASM_ERROR(line_counter, (ERR_FAILED_TO_RECORD_EXTERN));
					return FAILURE;
				}
			}
			else {
				target_address = symbol_node->symbol_table_entry.value;
				*machine_code |= ((unsigned long)(target_address & ADDRESS_MASK));
			}
			break;
		case HLT:
			break;
		default:
			ASM_ERROR(line_counter, (INVALID_J_OPCODE));
			return FAILURE;
		}
		break;
	default:
		ASM_ERROR(line_counter, (ERR_UNKNOWN_INSTRUCTION));
		return FAILURE;
	}
	return SUCCESS;
}


