#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "consts.h"
#include "utils.h"
#include "symbol_table.h"
#include "messages.h"

/* function prototypes for internal handlers */
Status handleDataDirective(char **line, char *directive, Boolean has_label, char *label_name,SymbolTable *symbol_table, int *dc, unsigned char *data_image, int line_counter);
Status handleEDirective(char **line, char *directive, Boolean has_label, char *label_name,SymbolTable *symbol_table, unsigned char *data_image, int line_counter);
Status handleInstruction(char **line, char *instruction_name, Boolean has_label, char *label_name, SymbolTable *symbol_table, int *ic, unsigned long* code_image, int line_counter);
void updateDataSymbolAddresses(SymbolTable symbol_table, int icf);


Status firstPass(char *file_base_name, SymbolTable* symbol_table, int* ic, int* dc, unsigned char *data_image, unsigned long* code_image, int *line_map){
	FILE *macro_file;
	char* macro_file_name;

	char current_line[MAX_SINGLE_LINE_LENGTH + 4]; /* buffer for current line (with extra space for handling special edge cases with line length)*/
	char *current_line_ptr; /* a pointer used to look at the line */
	char token[MAX_TOKEN_LENGTH]; /* a buffer for holding the current line token */
	int token_len;
	char label_name[MAX_LABEL_LENGTH];


	Boolean has_label =  FALSE; /* track if we have a label definition in line*/
	Status pass_status = SUCCESS; /* track status of the whole first pass stage*/
	int line_counter = 0; /*counter used to follow am lines*/
	int line_in_as; /*used in start of iteration to translate am line to as line */

	/* get the am file name and open it*/
	macro_file_name = createFileName(file_base_name, MACRO_ASSEMBLY_FILE);
	if (macro_file_name == NULL) {
		return FAILURE;
	}

	if ((macro_file = fopen(macro_file_name, "r")) == NULL){
		fprintf(stderr, ERR_FILE_OPERATION_FAILED, "open", "macro", macro_file_name);
		free(macro_file_name);
		return FAILURE;
	}


	/* start ic and dc for file*/
	*ic = IC_INITIAL_VALUE;
	*dc = DC_INITIAL_VALUE;

	/* iterate over file line by line*/
	while (fgets(current_line, sizeof(current_line), macro_file) != NULL){
		/* get the as line number for logging from the line map */
		line_counter++;
		line_in_as = line_map[line_counter];

		/* initialize iteration variables and clean buffers*/
		has_label = FALSE;
		current_line_ptr = current_line;
		memset(label_name, 0, sizeof(label_name));
		memset(token, 0, sizeof(token));

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
					token_len = strlen(token);

					/* if line defines a token, raise token flag, check if its name is valid, and check if the line has content besides the label definition*/
					if (isLabelDef(token)){
						has_label = TRUE;
						token[token_len - 1] = END_OF_STRING;
						if (!isValidLabel(token)) {
							ASM_ERROR(line_in_as, (ERR_INVALID_LABEL_NAME, token));
							pass_status = FAILURE;
							getNextToken(&current_line_ptr, token);
						}
						else{
							strcpy(label_name, token);
							if (!getNextToken(&current_line_ptr, token)){
								ASM_ERROR(line_in_as, (ERR_INCOMPLETE_LABEL));
								pass_status = FAILURE;
							}
						}
					}
					/* there is no token definition, so we need to check what the line holds*/
					if (token[0] != END_OF_STRING){
						/* handle data directive*/
						if(isDataDirective(token)){
							if(handleDataDirective(&current_line_ptr, token, has_label, label_name, symbol_table, dc, data_image, line_in_as) == FAILURE){
								pass_status = FAILURE;
							}
						}
						/* handle E directive (for now we actually handle the extern directive, but we look at both E directives here */
						else if (isExternDirective(token) || isEntryDirective(token)) {
							if (handleEDirective(&current_line_ptr, token, has_label, label_name, symbol_table, data_image, line_in_as) == FAILURE) {
								pass_status = FAILURE;
							}
						}
						/* handle instruction*/
						else if(isInstruction(token)){
							if(handleInstruction(&current_line_ptr, token, has_label, label_name, symbol_table, ic, code_image, line_in_as) == FAILURE){
								pass_status = FAILURE;
							}
						}
						/* whatever is in this line is unknown in our assembly language*/
						else{
							ASM_ERROR(line_in_as, (ERR_UNKNOWN_INST_OR_DIR, token));
							pass_status = FAILURE;
						}
					}
				}
			}

		}
	}

	/* close file, free file name */
	fclose(macro_file);
	free(macro_file_name);

	if (pass_status == FAILURE){
		return FAILURE;
	}
	/*if we finish successfully, update the symbol table's data entries */
	else{
		updateDataSymbolAddresses(*symbol_table, *ic);
		return SUCCESS;
	}
}

/*
 * handle data directives (.db, .dh, .dw, .asciz)
 * parse the operands, validate them, and encode them to machine code i the data image
 * in addition, updates the data counter
 * line: pointer to the current position in the line buffer
 * directive: data directive used
 * has_label: is the line starting with a label
 * symbol_table: pointer to the symbol table
 * dc: pointer to the data counter
 * data_image: the data_image array
 * line_counter: the current line in the as file as extracted by the line map
 *
 * return SUCCESS if parsed and encoded correctly, FAILURE on syntax or memory errors.
 */
Status handleDataDirective(char **line, char *directive, Boolean has_label, char *label_name,SymbolTable *symbol_table, int *dc, unsigned char *data_image, int line_counter){
	SymbolTableEntry data_entry = {0}; /* the symbol struct of new entry*/

	/* pointers used to find quotes in line*/
	char * first_quote;
	char * last_quote;
	char * line_ptr;

	int tmp_dc = *dc; /*hold dc value for  easier data image navigation*/
	int data_size; /* holds the size of the current data directive content in bytes*/

	/* variables used in operand extraction*/
	char operands[MAX_OPERANDS_PER_LINE][MAX_TOKEN_LENGTH];
	int operand_count = 0;

	int i;
	long data_content;
	long data_content_limit_min;
	long data_content_limit_max;
	int byte_index; /* iteration value used to determine which byte in content are we in*/
	char* endp; /* pointer used in strtol*/
	char extra_word[MAX_TOKEN_LENGTH]; /* a buffer for holding extranous words if found*/

	/* check memory size and limits based on directive*/
	data_size = (strcmp(directive, DB_DIRECTIVE) == 0) ? DB_SIZE : (strcmp(directive, DH_DIRECTIVE) == 0) ? DH_SIZE : DW_SIZE;
	data_content_limit_min = (strcmp(directive, DB_DIRECTIVE) == 0) ? SCHAR_MIN : (strcmp(directive, DH_DIRECTIVE) == 0) ? SHRT_MIN : INT_MIN;
	data_content_limit_max = (strcmp(directive, DB_DIRECTIVE) == 0) ? SCHAR_MAX : (strcmp(directive, DH_DIRECTIVE) == 0) ? SHRT_MAX : INT_MAX;


	/*if a label is present, add it to the symbol table with the data attributes */
	if (has_label) {
		if (findSymbol(*symbol_table, label_name) == NULL){
			strcpy(data_entry.symbol, label_name);
			data_entry.value = *dc;
			data_entry.attributes = DATA;
			if(addEntryToSymbolTable(symbol_table, data_entry) == FAILURE){
				ASM_ERROR(line_counter, (ERR_DATA_DIRECTIVE_SYMBOL_ADDITION_FAILED));
				return FAILURE;
			}
		}
		else{
			ASM_ERROR(line_counter, (ERR_SYMBOL_ALREADY_DEFINED, label_name));
			return FAILURE;
		}
	}

	/* if directive is asciz string*/
	if (strcmp(directive, ASCIZ_DIRECTIVE) == 0) {
		/* check if there is a quote at all in the string */
		first_quote = strchr(*line, STRING_WRAPPER);
		if(first_quote == NULL){
			ASM_ERROR(line_counter, (ERR_MISSING_QUOTES));
			return FAILURE;
		}
		/* find first and last quote, if they match we added only one quote*/
		last_quote = strrchr(*line, STRING_WRAPPER);
		if(last_quote == first_quote){
			ASM_ERROR(line_counter, (ERR_MISSING_CLOSING_QUOTES));
			return FAILURE;
		}
		/* check if there are any other quotes beside the first and last one*/
		if (strchr(first_quote + 1, STRING_WRAPPER) != last_quote) {
			ASM_ERROR(line_counter, (ERR_TOO_MANY_QUOTES_IN_STRING));
			return FAILURE;
		}
		/* copy the the characters from the string to the data image*/
		line_ptr = first_quote + 1;
		while(*line_ptr != STRING_WRAPPER){
			/* put character, iterate in line, and add to temporary dc*/
			data_image[tmp_dc] = *line_ptr;
			line_ptr++;
			tmp_dc++;
		}
		/* add null terminator and update the dc to hold all the new string data plus the eos*/
		data_image[tmp_dc] = END_OF_STRING;
		*dc = tmp_dc + 1;

		/*cehck if there is more content after close quote in asciz directive */
		*line = last_quote + 1;
		if (getNextToken(line, extra_word)) {
			ASM_ERROR(line_counter, (ERR_EXTRA_CHARACTERS_AFTER_ASCIZ));
			return FAILURE;
		}
	}
	/* data directive is d directive*/
	else{
		/* get operands for directive*/
		if (extractOperands(line, operands, &operand_count, line_counter) == FAILURE){
			return FAILURE;
		}
		if (operand_count == 0) {
			ASM_ERROR(line_counter, (ERR_MISSING_OPERANDS_DATA_DIRECTIVE));
			return FAILURE;
		}

		/* for each numeric operand, convert to a number and check that its in its datatype bounds*/
		for(i = 0; i < operand_count; i++){
			data_content = strtol(operands[i], &endp, 10);
			if (operands[i] == endp || *endp != END_OF_STRING){
				ASM_ERROR(line_counter, (ERR_INVALID_OPRANDS, directive));
				return FAILURE;
			}

			if (data_content > data_content_limit_max || data_content < data_content_limit_min){
				ASM_ERROR(line_counter, (ERR_DATA_NOT_FIT_FOR_TYPE, data_content, directive));
				return FAILURE;
			}

			/* encode each byte in 32 bit number based on the number size*/
			for (byte_index = 0; byte_index < data_size; byte_index++) {
				data_image[*dc + byte_index] = (data_content >> (byte_index * BITS_IN_BYTE)) & BYTE_MASK;
			}
			/* add datasize to dc*/
			*dc += data_size;
		}
	}
	return SUCCESS;
}

/*
 * handle extern and entry directives
 * parse the oprand, validate it, add to the extern table if necessary
 * line: pointer to the current position in the line buffer
 * directive: the directive string
 * has_label: is the line starting with a label
 * label_name: name of the label (if has_label is TRUE)
 * symbol_table: pointer to the symbol table
 * data_image: data image array
 * line_counter: the current line in the as file as extracted by the line map
 *
 * return SUCCESS if valid, FAILURE on syntax errors or symbol collisions
 */
Status handleEDirective(char **line, char *directive, Boolean has_label, char *label_name,SymbolTable *symbol_table, unsigned char *data_image, int line_counter){
	/* variables used in operand extraction*/
	char operands[MAX_OPERANDS_PER_LINE][MAX_TOKEN_LENGTH];
	int operand_count = 0;

	SymbolTableNode *existing_symbol;
	SymbolTableEntry new_entry = {0};

	/* if a label is defined before extern, warn about it but to not exit */
	if (has_label) {
		ASM_WARNING(line_counter, (WARN_LABEL_BEFORE_DIRECTIVE, label_name, directive));
	}

	/*get the extern oprands and check exactly one is provided*/
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

	/* if extern directive*/
	if (strcmp(directive, EXTERN_DIRECTIVE) == 0){
		/* look for extern value in the symbol table*/
		existing_symbol = findSymbol(*symbol_table, operands[0]);

		/* if symbol doest not exist, add it to the symbol table and return SUCCESS if done successfully */
		if(existing_symbol == NULL){
			strcpy(new_entry.symbol, operands[0]);
			new_entry.value = 0;
			new_entry.attributes = EXTERNAL;

			if (addEntryToSymbolTable(symbol_table, new_entry) == FAILURE) {
				return FAILURE;
			}
		}

		else{
			/* check if symbol is already defined as code or data, which is illegal*/
			if(existing_symbol->symbol_table_entry.attributes & CODE){
				ASM_ERROR(line_counter, (ERR_EXISTING_EXTERN_SYBOL_EXISTS, existing_symbol->symbol_table_entry.symbol, "code"));
				return FAILURE;
			}
			if(existing_symbol->symbol_table_entry.attributes & DATA){
				ASM_ERROR(line_counter, (ERR_EXISTING_EXTERN_SYBOL_EXISTS, existing_symbol->symbol_table_entry.symbol, "data"));
				return FAILURE;
			}
		}
		/* the symbol we saw is not data or code, but is already in the table, so we return SUCCESS*/
		return SUCCESS;
	}

	/* if entry, will be fully handled in second pass, so we ignore it and return SUCCESS*/
	else if (strcmp(directive, ENTRY_DIRECTIVE) == 0) {
		return SUCCESS;
	}
	return FAILURE;
}

/*
 * parse and encode instruction to machine code.
 * specifically, turn them into 32 bit values like explained in the manual.
 * parse instruction and operands, validate them, and encode them to machine code in the code image.
 * in addition, if there is a label, add it to the symbol table
 *
 *
 * line: pointer to the current position in the line buffer
 * instruction_name: name  of  instruction
 * has_label: is the line starting with a label
 * label_name: name of the label (if has_label is TRUE)
 * symbol_table: pointer to the symbol table
 * ic pointer to the instruction counter
 * data_image: code image array
 * line_counter: the current line in the as file as extracted by the line map
 *  *
 * return SUCCESS if parsed and encoded correctly, FAILURE on syntax or operand errors
 */
Status handleInstruction(char **line, char *instruction_name, Boolean has_label, char *label_name, SymbolTable *symbol_table, int *ic, unsigned long* code_image, int line_counter){
	SymbolTableEntry code_entry;
	Instruction * instruction;
	char operands[MAX_OPERANDS_PER_LINE][MAX_TOKEN_LENGTH];
	int operand_count = 0;
	unsigned long machine_code = 0;
	int rs = 0, rt = 0, rd = 0;
	int immed = 0;
	int reg = 0;
	long address = 0;
	if (has_label) {
		if (findSymbol(*symbol_table, label_name) == NULL){
			strcpy(code_entry.symbol,label_name);
			code_entry.value = *ic;
			code_entry.attributes = CODE;
			if(addEntryToSymbolTable(symbol_table, code_entry) == FAILURE){
				ASM_ERROR(line_counter, (ERR_SYMBOL_ALREADY_DEFINED, label_name));
				return FAILURE;
			}
		}
		else{
			ASM_ERROR(line_counter, (ERR_SYMBOL_ALREADY_DEFINED, label_name));
			return FAILURE;
		}
	}
	instruction = getInstruction(instruction_name);
	if (instruction == NULL) {
		ASM_ERROR(line_counter, (ERR_UNKNOWN_INSTRUCTION_NAME, instruction_name));
		return FAILURE;
	}

	if (extractOperands(line, operands, &operand_count, line_counter) == FAILURE) {
		return FAILURE;
	}

	if (operand_count < instruction->oprands) {
		ASM_ERROR(line_counter, (ERR_INSTRUCTION_OPRAND_COUNT_LOW, instruction->name, instruction->oprands, operand_count));
		return FAILURE;
	}

	else if (operand_count > instruction->oprands) {
		ASM_ERROR(line_counter, (ERR_INSTRUCTION_OPRAND_COUNT_HIGH, instruction->name, instruction->oprands, operand_count));
		return FAILURE;
	}

	machine_code |= (instruction->opcode << OPCODE_SHIFT);

	switch (instruction->type) {
	case R_TYPE:
		switch (instruction->opcode){
		case R_INSTRUCTIONS_ARITHMATIC_OPCODES:
			if (!parseRegister(operands[0], &rs) || !parseRegister(operands[1], &rt) || !parseRegister(operands[2], &rd)){
				ASM_ERROR(line_counter, (ERR_OPERAND_MUST_BE_VALID_REGISTER, instruction->name));
				return FAILURE;
			}
			break;

		case R_INSTRUCTIONS_MEMORY_OPCODES:
			if (!parseRegister(operands[0], &rs) || !parseRegister(operands[1], &rd)) {
				ASM_ERROR(line_counter, (ERR_OPERAND_MUST_BE_VALID_REGISTER, instruction->name));
				return FAILURE;
			}
			break;

		default:
			ASM_ERROR(line_counter, (INVALID_R_OPCODE));
			return FAILURE;
		}

		machine_code |= (rs << RS_SHIFT);
		machine_code |= (rt << RT_SHIFT);
		machine_code |= (rd << RD_SHIFT);
		machine_code |= (instruction->funct << FUNCT_SHIFT);
		break;

		case I_TYPE:
			switch (instruction->opcode){
			case ADDI: case SUBI: case ANDI: case ORI: case NORI: case LB: case SB: case LW: case SW: case LH: case SH:
				if (!parseRegister(operands[0], &rs) || !parseRegister(operands[2], &rt) || !parseImmediate(operands[1], &immed)) {
					ASM_ERROR(line_counter, (ERR_INVALID_OPRANDS, instruction->name));
					return FAILURE;
				}
				break;

			case BNE: case BEQ: case BLT: case BGT:
				if (!parseRegister(operands[0], &rs) || !parseRegister(operands[1], &rt)) {
					ASM_ERROR(line_counter, (ERR_OPERANDS_ARE_NOT_REGISTERS, instruction->name));
					return FAILURE;
				}
				if (!isValidLabel(operands[2])) {
					ASM_ERROR(line_counter, (ERR_OPERAND_IS_NOT_LABEL, instruction->name));
					return FAILURE;
				}
				break;

			default:
				ASM_ERROR(line_counter, (INVALID_I_OPCODE));
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
						ASM_ERROR(line_counter, (ERR_INVALID_REGISTER_JMP));
						return FAILURE;
					}
					address = rs;
					reg = 1;
				}
				else{
					if (!isValidLabel(operands[0])) {
						ASM_ERROR(line_counter, (ERR_INVALID_LABEL_JMP));
						return FAILURE;
					}
				}
				break;

			case LA: case CALL:
				if (!isValidLabel(operands[0])) {
					ASM_ERROR(line_counter, (ERR_OPERAND_MUST_BE_VALID_LABEL, instruction->name));
					return FAILURE;
				}
				break;

			default:
				ASM_ERROR(line_counter, (INVALID_J_OPCODE));
				return FAILURE;
			}
			machine_code |= (reg << REG_JUMP_SHIFT);
			machine_code |= (address & ADDRESS_MASK);
			break;

			default:
				ASM_ERROR(line_counter, (ERR_UNKNOWN_INSTRUCTION_TYPE));
				return FAILURE;
	}

	code_image[(*ic - IC_INITIAL_VALUE) / INSTRUCTION_BYTES_SIZE] = machine_code;

	*ic += INSTRUCTION_BYTES_SIZE;
	return SUCCESS;
}

/*
 * update the addresses of data symbols in symbol table after first pass,
 * by adding to tit the final value of the instruction counter
 *
 * symbol_table: the symbol table to update
 * icf: the final instruction counter value
 */
void updateDataSymbolAddresses(SymbolTable symbol_table, int icf) {
	SymbolTableNode *current = symbol_table; /* a pointer to the symbol node's linked list */

	/* if the table is empty, break immediately */
	if (symbol_table == NULL){
		return;
	}

	/* iterate over the linked list, if a symbol with the data attribute is found, add the ifc to it */
	while (current != NULL) {
		if (current->symbol_table_entry.attributes & DATA) {
			current->symbol_table_entry.value += icf;
		}
		current = current->next_entry;
	}
}

