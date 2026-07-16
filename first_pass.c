
#include "consts.h"
#include "utils.h"


Boolean isValidLabel(char * str){

}



Status firstPass(char * file_base_name){
	int IC = IC_INITIAL_VALUE;
	int DC = DC_INITIAL_VALUE;
	char * current_line[MAX_SINGLE_LINE_LENGTH + 2];
	int line_counter = 0;

	while (fgets(current_line, sizeof(line), file) != NULL){
		line_counter++;

		//TODO: split the line to parts and trim spaces

		//TODO: check if the line is empty or is a comment

		//TODO: check if the first part of the line is a valid label
	}
}
