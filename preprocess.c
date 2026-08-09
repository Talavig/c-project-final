#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "consts.h"
#include "utils.h"
#include "macro_table.h"
#include "messages.h"

Status preprocessScript(char *file_base_name, int *line_map) {
	FILE *input_file;
	FILE *output_file;
	char *input_file_name = NULL;
	char *output_file_name = NULL;

	char current_line[MAX_SINGLE_LINE_LENGTH + 4]; /* buffer for current line (with extra space for handling special edge cases with line length)*/
	char *current_line_ptr; /* a pointer used to look at the line */
	char first_word[MAX_TOKEN_LENGTH];
	char macro_name[MAX_TOKEN_LENGTH];
	char extra_word[MAX_TOKEN_LENGTH]; /* a buffer for holding extranous words if found*/

	Boolean is_macro = FALSE; /* track if we are currently inside a mcro block*/
	Status run_status = SUCCESS; /* track status of the whole preprocessing stage*/

	int as_line_counter = 0; /* counts lines in the as file*/
	int am_line_counter = 0; /* counts file in the newly created am file*/

	MacroTable macro_table = NULL; /* created macro table for current file*/
	MacroTableNode *current_entry = NULL; /* pointer to the current macro in the linked list*/
	MacroTableNode *found_macro = NULL; /* used for looking up macros in the macro table*/

	char *macro_content = NULL;
	char *content_copy = NULL;
	char *line_tokenizer = NULL;

	Boolean is_macro_end; /* used to follow if we finished with a macro*/
	Boolean is_a_macro_related_line; /* used to follow if current line was handled as a part of a macro */

	/* create output file names and open input & output files for read & write respectively*/
	input_file_name = createFileName(file_base_name, INPUT_ASSEMBLY_FILE);
	output_file_name = createFileName(file_base_name, MACRO_ASSEMBLY_FILE);

	if (input_file_name == NULL || output_file_name == NULL) {
		if (input_file_name) free(input_file_name);
		if (output_file_name) free(output_file_name);
		return FAILURE;
	}

	if ((input_file = fopen(input_file_name, "r")) == NULL) {
		fprintf(stderr, ERR_FILE_OPERATION_FAILED, "open", "input", input_file_name);
		free(input_file_name);
		free(output_file_name);
		return FAILURE;
	}

	if ((output_file = fopen(output_file_name, "w")) == NULL) {
		fprintf(stderr, ERR_FILE_OPERATION_FAILED, "create", "macro", output_file_name);
		fclose(input_file);
		free(input_file_name);
		free(output_file_name);
		return FAILURE;
	}

	/* iterate over the lines of the input file*/
	while (fgets(current_line, sizeof(current_line), input_file) != NULL) {
		/* advance th original file's line counter*/
		as_line_counter++;

		/* set flow flags for line*/
		is_macro_end = FALSE;
		is_a_macro_related_line = FALSE;

		/* check if line is too long*/
		if (isLineTooLong(current_line, input_file)) {
			ASM_ERROR(as_line_counter, (ERR_LINE_TOO_LONG));
			run_status = FAILURE;
		}
		else {
			/* check if currently in middle of expanding a macro*/
			if (is_macro) {
				current_line_ptr = skipWhitespaces(current_line);

				/* we skip empty lines and comments in am file creating as defined in manual*/
				if (!isEmptyOrComment(current_line_ptr)) {
					getNextToken(&current_line_ptr, first_word);

					/* check if this line closes the macro definition*/
					if (strcmp(first_word, MACRO_END) == 0) {
						/* raise macro end flag, check if there are extra words after macro end*/
						is_macro_end = TRUE;
						if (getNextToken(&current_line_ptr, extra_word) == TRUE) {
							ASM_ERROR(as_line_counter, (ERR_EXTRA_CHARS_MACRO_END));
							run_status = FAILURE;
						}
						else {
							/* otherwise annotate we are now not in a macro, and reset the entry pointer*/
							is_macro = FALSE;
							current_entry = NULL;
						}
					}
				}

				/* we know we are in an ongoing macro, so we add the line to the macro table*/
				if (!is_macro_end) {
					if (addLineToMacro(current_entry, current_line) == FAILURE) {
						run_status = FAILURE;
					}
				}
			}
			/* we are not reading a macro now*/
			else {
				current_line_ptr = skipWhitespaces(current_line);

				/* check if the line is not empty or a comment, as we dont care about these here*/
				if (!isEmptyOrComment(current_line_ptr)) {
					getNextToken(&current_line_ptr, first_word);

					/* check if this line starts a new macro and raise the appropriate flag*/
					if (strcmp(first_word, MACRO_START) == 0) {
						is_a_macro_related_line = TRUE;

						/* validate a macro name exists*/
						if (getNextToken(&current_line_ptr, macro_name) == FALSE) {
							ASM_ERROR(as_line_counter, (ERR_NO_MACRO_NAME));
							run_status = FAILURE;
						/* validate macro name is the only other text in line*/
						} else if (getNextToken(&current_line_ptr, extra_word) == TRUE) {
							ASM_ERROR(as_line_counter, (ERR_EXTRA_TEXT_AFTER_MACRO_NAME));
							run_status = FAILURE;
						/* check if the macro name is a non reserved name*/
						} else if (isReservedWord(macro_name)) {
							ASM_ERROR(as_line_counter, (ERR_MACRO_NAME_RESERVED_WORD));
							run_status = FAILURE;
						/* check if the macro name is not unique*/
						} else if (findMacro(macro_table, macro_name) != NULL) {
							ASM_ERROR(as_line_counter, (ERR_MACRO_ALREADY_DEFINED, macro_name));
							run_status = FAILURE;
						} else {
							/* we are starting to expand a valid macro, raise the flag, add tot he table and set it as the currently expanded macro*/
							is_macro = TRUE;
							if (addMacroToTable(&macro_table, macro_name) == FAILURE) {
								run_status = FAILURE;
							} else {
								current_entry = findMacro(macro_table, macro_name);
							}
						}
					}
					/* check if this line is a call to the macro*/
					else {
						found_macro = findMacro(macro_table, first_word);
						if (found_macro != NULL) {
							is_a_macro_related_line = TRUE;
							macro_content = found_macro->macro_table_entry.content;

							/* if the macro is not empty, copy its content to a temp buffer and go over it line by line, copying its contents to the as file*/
							if (macro_content != NULL) {
								content_copy = (char *)malloc(strlen(macro_content) + 1);
								if (content_copy == NULL) {
									fprintf(stderr, ERR_MEM_ALLOC_FAILED, "macro content copy");
									run_status = FAILURE;
								} else {
									strcpy(content_copy, macro_content);
									line_tokenizer = strtok(content_copy, "\n");

									while (line_tokenizer != NULL) {
										/* increment the am line counter and add it to the line mapping for backtracking to macro original line*/
										am_line_counter++;
										if (am_line_counter < MAX_ASSEMBLY_LINE_COUNT) {
											line_map[am_line_counter] = as_line_counter;
										}
										/* add the line to the am file and go to the next line in macro*/
										fprintf(output_file, "%s\n", line_tokenizer);
										line_tokenizer = strtok(NULL, "\n");
									}
									/* free the copied content*/
									free(content_copy);
									content_copy = NULL;
								}
							}
						}
					}
				}

				/* found line has nothing to do with macro, so we add it to the line map and add to the am file*/
				if (!is_a_macro_related_line) {
					am_line_counter++;
					if (am_line_counter < MAX_ASSEMBLY_LINE_COUNT) {
						line_map[am_line_counter] = as_line_counter;
					}
					fputs(current_line, output_file);
				}
			}
		}
	}

	/* close files, free filenames and free macro table*/
	fclose(input_file);
	fclose(output_file);
	free(input_file_name);

	if (run_status == FAILURE) {
		/* if the preprocess has failed, do not create an am file*/
		remove(output_file_name);
	}

	free(output_file_name);
	freeMacroTable(&macro_table);

	return run_status;
}
