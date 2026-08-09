#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"
#include "utils.h"
#include "symbol_table.h"
#include "extern_table.h"
#include "messages.h"
#include "second_pass.h"

/* function prototypes for internal handlers */
static Status handleEntryDirective(char **line, SymbolTable *symbol_table, int line_counter);
static Status encodeCodeTraversalInstructions(char **line, char *instruction_name, SymbolTable *symbol_table, int ic, unsigned long *code_image, ExternTable *extern_table, int line_counter);

Status secondPass(char *macro_file_name, SymbolTable *symbol_table, unsigned long *code_image, ExternTable *extern_table, int *line_map) {
	FILE *macro_file;

	char current_line[MAX_SINGLE_LINE_LENGTH + 4]; /* buffer for current line (with extra space for handling special edge cases with line length)*/
	char *current_line_ptr; /* a pointer used to look at the line */
	char token[MAX_TOKEN_LENGTH]; /* a buffer for holding the current line token */

	int second_pass_ic = IC_INITIAL_VALUE; /* ic reset used for address/offset calculations */

	Status pass_status = SUCCESS; /* track status of the whole second pass stage*/
	int line_counter = 0; /*counter used to follow am lines*/
	int line_in_as; /*used in start of iteration to translate am line to as line */

	/* open am file*/
	if ((macro_file = fopen(macro_file_name, "r")) == NULL){
		fprintf(stderr, ERR_FILE_OPERATION_FAILED, "open", "macro", macro_file_name);
		free(macro_file_name);
		return FAILURE;
	}

	/* iterate over file line by line*/
	while (fgets(current_line, sizeof(current_line), macro_file) != NULL) {
		/* get the as line number for logging from the line map */
		line_counter++;
		line_in_as = line_map[line_counter];
		current_line_ptr = current_line;

		/* check if line is too long*/
		if (isLineTooLong(current_line, macro_file)) {
			ASM_ERROR(line_in_as, (ERR_LINE_TOO_LONG));
			pass_status = FAILURE;
		}
		else{
			cleanLineEnding(current_line);
			current_line_ptr = skipWhitespaces(current_line_ptr);

			/* if the line is not empty, extract a token from the line*/
			if (!isEmptyOrComment(current_line_ptr)) {
				if (getNextToken(&current_line_ptr, token)) {
					if (isLabelDef(token)) {
						if (!getNextToken(&current_line_ptr, token)) {
							token[0] = END_OF_STRING;
						}
					}

					if (token[0] != END_OF_STRING) {
						/* handle entry directive*/
						if (isEntryDirective(token)) {
							if (handleEntryDirective(&current_line_ptr, symbol_table, line_in_as) == FAILURE) {
								pass_status = FAILURE;
							}
						}
						/* handle instruction encoding*/
						else if (isInstruction(token)) {
							if (encodeCodeTraversalInstructions(&current_line_ptr, token, symbol_table, second_pass_ic, code_image, extern_table, line_in_as) == FAILURE) {
								pass_status = FAILURE;
							}
							else{
								/* advance ic copy */
								second_pass_ic += INSTRUCTION_BYTES_SIZE;
							}
						}
						/* if the token is unknown (not data or extern), return error*/
						else if (!isDataDirective(token) && !isExternDirective(token)){
							ASM_ERROR(line_in_as, (ERR_UNKNOWN_INST_OR_DIR, token));
							pass_status = FAILURE;
						}
					}
				}
			}
		}
	}
	/* close file */
	fclose(macro_file);
	return pass_status;
}

/*
 * handles the .entry directive by marking the symbol in the symbol table
 * validates that the symbol exists in the file and is not already marked as EXTERNAL
 *
 * line: pointer to the current position in the line buffer
 * symbol_table: pointer to the symbol table
 * line_counter: original line number in the .as file (for errors)
 * return SUCCESS if successfully marked as entry, FAILURE on conflicts or missing symbols
 */
static Status handleEntryDirective(char **line, SymbolTable *symbol_table, int line_counter) {
	char label_name[MAX_TOKEN_LENGTH];
	SymbolTableNode *symbol_node;

	/* extract the label to be marked as ENTRY */
	if (!getNextToken(line, label_name)) {
		ASM_ERROR(line_counter, (ERR_MISSING_LABEL_AFTER_ENTRY));
		return FAILURE;
	}

	/* look for label in the symbol table (it must exist from the first pass) */
	symbol_node = findSymbol(*symbol_table, label_name);
	if (symbol_node == NULL) {
		ASM_ERROR(line_counter, (ERR_ENTRY_LABEL_NOT_DEFINED, label_name));
		return FAILURE;
	}

	/* symbol can't be both externally provided and an entry point for others */
	if (symbol_node->symbol_table_entry.attributes & EXTERNAL) {
		ASM_ERROR(line_counter, (ERR_SYMBOL_BOTH_ENTRY_AND_EXTERN, label_name));
		return FAILURE;
	}

	/* add to symbole the entry attribute */
	symbol_node->symbol_table_entry.attributes |= ENTRY;

	return SUCCESS;
}

/**
 * encode the missing memory addresses for branch and jump instructions
 * parse the line to extract the label operand, search for it in the symbol table,
 * and add its needed offset/address to the code image
 *
 * line: pointer to the current position in the line buffer
 * instruction_name: name of the instruction
 * symbol_table: pointer to the symbol table
 * ic: current instruction counter value
 * code_image: the code image array
 * extern_table: pointer to the extern table
 * line_counter: original line number in the .as file (for errors)
 * return: SUCCESS if address resolution is successful, FAILURE if the label is missing or illegal.
 */
static Status encodeCodeTraversalInstructions(char **line, char *instruction_name, SymbolTable *symbol_table, int ic, unsigned long *code_image, ExternTable *extern_table, int line_counter){
	Instruction *instruction;
	/* variables used in operand extraction*/
	char operands[MAX_OPERANDS_PER_LINE][MAX_TOKEN_LENGTH];
	int operand_count = 0;
	SymbolTableNode *symbol_node;
	ExternTableEntry extern_entry;

	/* variables for encoding instruction parts*/
	char *label_name;
	long target_address;
	int offset;

	/* get the array index for current instruction in the code image*/
	int code_index = (ic - IC_INITIAL_VALUE) / INSTRUCTION_BYTES_SIZE;
	unsigned long *machine_code = &code_image[code_index];

	instruction = getInstruction(instruction_name);
	if (instruction == NULL) {
		ASM_ERROR(line_counter, (ERR_UNKNOWN_INSTRUCTION_NAME, instruction_name));
		return FAILURE;
	}

	if (extractOperands(line, operands, &operand_count, line_counter) == FAILURE) {
		return FAILURE;
	}

	switch (instruction->type) {
	case R_TYPE:
		/* all R types are handled in first pass*/
		break;
	case I_TYPE:
		switch (instruction->opcode) {
		/* check if branched label exists in the symbol table*/
		case BNE: case BEQ: case BLT: case BGT:
			label_name = operands[2];
			symbol_node = findSymbol(*symbol_table, label_name);
			if (symbol_node == NULL) {
				ASM_ERROR(line_counter, (ERR_UNDEFINED_LABEL_IN_BRANCH, label_name));
				return FAILURE;
			}

			/* according to manual, one may not branch to extern*/
			if (symbol_node->symbol_table_entry.attributes & EXTERNAL) {
				ASM_ERROR(line_counter, (ERR_BRANCH_TO_EXTERN, label_name));
				return FAILURE;
			}

			/* the branch offset it calculated as: (target) -(current_instruction)*/
			offset = symbol_node->symbol_table_entry.value - ic;

			/* encode the data to an I format like the manual requires*/
			*machine_code |= ((unsigned long)offset & IMMED_MASK);
			break;
		default:
			break;
		}
		break;
	case J_TYPE:
		switch (instruction->opcode) {
		case JMP:
			/* if jumping to register, the address was resolved in pass 1*/
			if (operands[0][0] == REGISTER_INDICATOR) {
				break;
			}
			/* this must now be a label*/
		case LA: case CALL:
			label_name = operands[0];
			symbol_node = findSymbol(*symbol_table, label_name);
			if (symbol_node == NULL) {
				ASM_ERROR(line_counter, (ERR_UNDEFINED_LABEL_J_TYPE, label_name));
				return FAILURE;
			}

			/* if jumping to an external symbol, encode 0 which is the default and document usage location */
			if (symbol_node->symbol_table_entry.attributes & EXTERNAL) {
				strcpy(extern_entry.symbol, label_name);
				extern_entry.address = ic;

				if (addEntryToExternTable(extern_table, extern_entry) == FAILURE) {
					ASM_ERROR(line_counter, (ERR_FAILED_TO_RECORD_EXTERN));
					return FAILURE;
				}
			}
			else {
				/* encode the data to an J format like the manual requires*/
				target_address = symbol_node->symbol_table_entry.value;
				*machine_code |= ((unsigned long)(target_address & ADDRESS_MASK));
			}
			break;
		case HLT:
			/* hlt has no oprands */
			break;
		default:
			ASM_ERROR(line_counter, (INVALID_J_OPCODE));
			return FAILURE;
		}
		break;
	default:
		ASM_ERROR(line_counter, (ERR_UNKNOWN_INSTRUCTION_TYPE));
		return FAILURE;
	}
	return SUCCESS;
}


