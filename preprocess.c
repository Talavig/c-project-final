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
	char* input_file_name;
	char* output_file_name;
	Boolean is_macro;
	char current_line[MAX_SINGLE_LINE_LENGTH] = {0};
	char first_word[MAX_LABEL_LENGTH + 1] = {0};
	char macro_name[MAX_LABEL_LENGTH + 1] = {0};
	Status run_status = SUCCESS;

	MacroTableEntry *macro_table = NULL;
	MacroTableEntry *current_entry = NULL;
	MacroTableEntry *found_macro = NULL;

	input_file_name = (char *)malloc(strlen(file_base_name) + 4);
	output_file_name = (char *)malloc(strlen(file_base_name) + 4);

	if (input_file_name == NULL || output_file_name == NULL) {
		fprintf(stderr, ERR_ALLOCATION_FAILED);
		free(input_file_name);
		free(output_file_name);
		return FAILURE;
	}

	is_macro = FALSE;

	strcpy(input_file_name, file_base_name);
	strcat(input_file_name, INPUT_ASSEMBLY_FILE);

	strcpy(output_file_name, file_base_name);
	strcat(output_file_name, MACRO_ASSEMBLY_FILE);

	if ((input_file = fopen(input_file_name, "r")) == NULL){
		fprintf(stderr, ERR_CANNOT_OPEN_FILE, input_file_name);
		free(input_file_name);
		free(output_file_name);
		return FAILURE;
	}

	if((output_file = fopen(output_file_name, "w")) == NULL){
		fprintf(stderr, ERR_CANNOT_CREATE_FILE, output_file_name);
		fclose(input_file);
		free(input_file_name);
		free(output_file_name);
		return FAILURE
	}

	while (fgets(current_line, sizeof(current_line), input_file) != NULL){

		/* increment line count, reset first word buffer */
		line_counter++;
		memset(first_word, 0, sizeof(first_word));

		/* check if the current line can be a valid assembly line*/
		if (parseLine(current_line, line_counter, first_word, input_file) == FAILURE){
			run_status = FAILURE;
		}

		/* skip empty lines\comments */
		else if (first_word[0] == END_OF_STRING){
			if (is_macro){
				if (addLineToMacro(current_macro, current_line) == FAILURE){
					run_status = FAILURE;
				}
			}
			else{
				fputs(current_line, output_file);
			}
		}

		/* currenly defining a macro */
		else if (is_macro){
			if (strcmp (first_word, MACRO_END) == 0){
				if (checkMacroLine(current_line) == FALSE){
					ASM_ERROR(line_counter, (ERR_EXTRA_CHARS_MACRO_END));
					run_status = FAILURE;
				}
				is_macro = FALSE;
				current_macro = NULL;
			}
			else {
				if (addLineToMacro(current_macro, current_line) == FAILURE){
					run_status = FAILURE;
				}
			}
		}

		/* currenly starting a mew macro */
		else if (strcmp(first_word, MACRO_START) == 0) {
			memset(macro_name, 0, sizeof(macro_name));
			sscanf(current_line, MACRO_START_CMD " %s", macro_name);

			if(!checkMacroName(macro_name) || checkMacroLine(current_line) == FALSE){
				ASM_ERROR(line_counter, (ERR_INVALID_MACRO_DEF, macro_name));
				run_status = FAILURE;
			}

			is_macro = TRUE;
			if (addMacroToTable(&macro_table, macro_name) == FAILURE) {
				run_status = FAILURE;
			}
			current_macro = findMacro(macro_table, macro_name);
		}

		/* currently expanding a macro */
		found_macro = findMacro(macro_table, first_word);
		if (found_macro != NULL) {
			fputs(found_macro->content, output_file);
		}

		else{
			fputs(current_line, output_file);
		}
	}
	fclose(input_file);
	fclose(output_file);
	free(input_file_name);
	free(output_file_name);
	freeMacroTable(macro_table);

	return run_status;
}


Status parseLine(char *line, int line_counter, char *first_word, FILE *input_file){
	char *ptr = line;
	int i = 0;
	int tmp_character;


	/* check single line length not passing 80 characters*/
	if (strchr(line, '\n') == NULL && !feof(input_file)){
		/* print an errpr and clean the buffer */
		ASM_ERROR(line_counter, ("Line exceeds maximum length of 80 characters."));

		while((tmp_character = fgetc(input_file))!= '\n' && tmp_character != EOF);
		return FAILURE;
	}
	 /* get to fist nonspace in line*/
	while (isspace(*ptr) && *ptr != '\n'){
		ptr++;
	}

	/* is the line is empty or is a comment */
	if(*ptr == '\n' || *ptr == END_OF_STRING || *ptr == COMMENT){
		first_word[0] = '\0';
		return SUCCESS;
	}

	while (!isspace(*ptr) && *ptr == END_OF_STRING && i < MAX_LABEL_LENGTH){
		first_word[i] = *ptr;
		ptr++;
		i++;
	}
	first_word[i] = '\0';

	return SUCCESS;
}
