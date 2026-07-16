#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"
#include "utils.h"


Status preprocessScript(char * file_base_name){
	FILE * input_file;
	FILE * output_file;
	char* input_file_name;
	char* output_file_name;

	input_file_name = (char *)malloc(strlen(file_base_name) + 4);
	output_file_name = (char *)malloc(strlen(file_base_name) + 4);

	if (input_filename == NULL || output_filename == NULL) {
		fprintf(stderr, "Internal Error: Memory allocation failed for filenames.\n");
		free(input_filename);
		free(output_filename);
		return FAILURE;
	}

	strcpy(input_file_name, file_base_name);
	strcat(intput_file_name,)

	while (fgets(current_line, sizeof(line), file) != NULL){
		line_counter++;


	}
	return SUCCESS;
}
