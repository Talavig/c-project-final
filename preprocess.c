#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "consts.h"
#include "utils.h"
#include "macro_table.h"
#include "messages.h"


Status preprocessScript(char * file_base_name){
	FILE * input_file;
	FILE * output_file;
	char* input_file_name = NULL;
	char* output_file_name = NULL;

	char current_line[MAX_SINGLE_LINE_LENGTH + 2];
	char *current_line_ptr;
	char first_word[MAX_TOKEN_LENGTH];
	char macro_name[MAX_TOKEN_LENGTH];
	char extra_word[MAX_TOKEN_LENGTH];

	Boolean is_macro = FALSE;
	Status run_status = SUCCESS;
	int line_counter = 0;

	MacroTable macro_table = NULL;
	MacroTableNode *current_entry = NULL;
	MacroTableNode *found_macro = NULL;

	input_file_name = createFileName(file_base_name, INPUT_ASSEMBLY_FILE);
	output_file_name = createFileName(file_base_name, MACRO_ASSEMBLY_FILE);

	if (input_file_name == NULL || output_file_name == NULL) {
		if (input_file_name){
			free(input_file_name);
		}
		if (output_file_name){
			free(output_file_name);
		}
		return FAILURE;
	}

	if ((input_file = fopen(input_file_name, "r")) == NULL){
		fprintf(stderr, ERR_FILE_OPERATION_FAILED, "open", "input", input_file_name);
		free(input_file_name);
		free(output_file_name);
		return FAILURE;
	}

	if((output_file = fopen(output_file_name, "w")) == NULL){
		fprintf(stderr, ERR_FILE_OPERATION_FAILED, "create", "macro", output_file_name);
		fclose(input_file);
		free(input_file_name);
		free(output_file_name);
		return FAILURE;
	}

	while (fgets(current_line, sizeof(current_line), input_file) != NULL){

		/* increment line count, reset first word buffer */
		line_counter++;
		current_line_ptr = current_line;

		/* check if the current line can be a valid assembly line*/
		if (isLineTooLong(current_line, input_file)) {
			ASM_ERROR(line_counter, (ERR_LINE_TOO_LONG));
			run_status = FAILURE;
		}

		else{
			current_line_ptr = skipWhitespaces(current_line_ptr);
			if (isEmptyOrComment(current_line_ptr)) {
				if (is_macro) {
					if (addLineToMacro(current_entry, current_line) == FAILURE) {
						run_status = FAILURE;
					}
					else{
						fputs(current_line, output_file);
					}
				}
			}
			else {
				getNextToken(&current_line_ptr, first_word);

				if (is_macro && strcmp(first_word, MACRO_END) == 0) {
					if (getNextToken(&current_line_ptr, extra_word) == TRUE) {
						ASM_ERROR(line_counter, (ERR_EXTRA_CHARS_MACRO_END));
						run_status = FAILURE;
					}
					else{
						is_macro = FALSE;
						current_entry = NULL;
					}
				}

				else if(is_macro){
					if (addLineToMacro(current_entry, current_line) == FAILURE) {
						run_status = FAILURE;
					}
				}

				else if (strcmp(first_word, MACRO_START) == 0){
					if (getNextToken(&current_line_ptr, macro_name) == FALSE) {
						ASM_ERROR(line_counter, (ERR_NO_MACRO_NAME));
						run_status = FAILURE;
					}
					else if (getNextToken(&current_line_ptr, extra_word) == TRUE) {
						ASM_ERROR(line_counter, (ERR_EXTRA_TEXT_AFTER_MACRO_NAME));
						run_status = FAILURE;
					}
					else if (isReservedWord(macro_name)) {
						ASM_ERROR(line_counter, (ERR_MACRO_NAME_RESERVED_WORD));
						run_status = FAILURE;
					}
					else if (findMacro(macro_table, macro_name) != NULL) {
						ASM_ERROR(line_counter, (ERR_MACRO_ALREADY_DEFINED, macro_name));
						run_status = FAILURE;
					}
					else{
						is_macro = TRUE;
						if (addMacroToTable(&macro_table, macro_name) == FAILURE) {
							run_status = FAILURE;
						}
						else{
							current_entry = findMacro(macro_table, macro_name);
						}
					}
				}
				else{
					found_macro = findMacro(macro_table, first_word);
					if (found_macro != NULL) {
						fputs(found_macro->macro_table_entry.content, output_file);
					}
					else{
						fputs(current_line, output_file);
					}
				}

			}
		}
	}

	fclose(input_file);
	fclose(output_file);
	free(input_file_name);
	freeMacroTable(&macro_table);

	if (run_status == FAILURE) {
		remove(output_file_name);
	}
	free(output_file_name);

	return run_status;
}
