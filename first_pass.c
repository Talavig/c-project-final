
#include "consts.h"
#include "utils.h"


Boolean isValidLabel(char * str){

}

else if (firstPass(file_base_name, &symbol_table, &ic, &dc) == FAILURE);


Status firstPass(char * file_base_name, SymbolTable* sybol_table, int* ic, int* dc){
	char * current_line[MAX_SINGLE_LINE_LENGTH + 2];
	int line_counter = 0;

	while (fgets(current_line, sizeof(line), file) != NULL){
		line_counter++;

		//TODO: split the line to parts and trim spaces

		//TODO: check if the line is empty or is a comment

		//TODO: check if the first part of the line is a valid label
	}
	return SUCCESS;
}
