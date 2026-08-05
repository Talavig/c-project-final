#include <stdio.h>
#include <string.h>


#include "consts.h"
#include "utils.h"
#include "symbol_table.h"


Status secondPass(char *file_name, SymbolTable *symbol_table, unsigned int[]* code_image, /* ExternList *ext_list */) {
	char * current_line[MAX_SINGLE_LINE_LENGTH + 2];
	int line_counter = 0;
	Status pass_status = SUCCESS;
	char *current_line_ptr;
	char token[MAX_TOKEN_LENGTH];
	int second_pass_ic = IC_INITIAL_VALUE;


	if ((macro_file = fopen(file_name, "r")) == NULL){
		fprintf(stderr, ERR_CANNOT_OPEN_FILE, macro_file_name);
		free(macro_file_name);
		return FAILURE
	}


	fclose(file);
	if(pass_status == FAILURE){
		return FAILURE;
	}



	return SUCCESS;











	return SUCCESS;
}
