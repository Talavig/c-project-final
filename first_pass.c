
#include "consts.h"
#include "utils.h"


Boolean isValidLabel(char * str){

}

else if (firstPass(file_base_name, &symbol_table, &ic, &dc) == FAILURE);


Status firstPass(char * file_base_name, SymbolTable* sybol_table, int* ic, int* dc){
	char * current_line[MAX_SINGLE_LINE_LENGTH + 2];
	int line_counter = 0;
	FILE *macro_file;
	char* macro_file_name;
	Status pass_status = SUCCESS;
	Boolean has_label =  FALSE;
	char label_name[MAX_LABEL_LENGTH];
	char *current_line_ptr;
	char token[MAX_TOKEN_LENGTH];
	int token_len;
	char operands[MAX_OPERANDS_PER_LINE][MAX_TOKEN_LENGTH];
	int operand_count = 0;


	macro_file_name = (char *)malloc(strlen(file_base_name) + 4);
	strcpy(macro_file_name, file_base_name);
	strcat(macro_file_name, INPUT_ASSEMBLY_FILE);

	if ((macro_file = fopen(macro_file_name, "r")) == NULL){
		fprintf(stderr, ERR_CANNOT_OPEN_FILE, macro_file_name);
		free(macro_file_name);
		return FAILURE
	}


	*ic = IC_INITIAL_VALUE;
	*dc = DC_INITIAL_VALUE;

	while (fgets(current_line, sizeof(line), file) != NULL){
		has_label = FALSE;
		current_line_ptr = current_line;
		memset(label_name, 0, sizeof(label_name));
		memset(token, 0, sizeof(token));
		line_counter++;

		current_line[strcspn(current_line, "\n")] = '\0';
		current_line_ptr = skipWhitespaces(current_line_ptr);
		if (!isEmptyOrComment(current_line_ptr)) {
			if (getNextToken(&current_line_ptr, token)) {
				token_len = strlen(token);
				if (isLabelDef(token)){
					has_label = TRUE;
					token[strlen(token) - 1] = '\0';
					strcpy(label_name, token);

					if (!getNextToken(&ptr, token)){
						ASM_ERROR(line_counter, (ERR_INCOMPLETE_LABEL));
						Status pass_status = FAILURE;
					}
				}
				else if(isDataDirective(token)){
					handleDataDirective();
				}
				else if(isExternDirective(token)){
					handleExternDirective();
				}
				/* entry directives are handled in teh second pass */
				else if(isInstruction(token)){
					if (has_label){

					}
					*ic += 4;
				}
				else{
					ASM_ERROR(line_counter, (ERR_UNKNOWN_INSTRUCTION, token));
					Status pass_status = FAILURE;
				}

			}
		}
	}

	fclose(macro_file);
	if (pass_status == FAILURE){
		return FAILURE;
	}
	else{

		return SUCCESS;
	}
}

Boolean isEmptyOrComment(char *current_line){
	return *(current_line == END_OF_STRING || *current_line == COMMENT) ? TRUE : FALSE;
}

Boolean isLabelDef(char *token){
	return (token[strlen(token) - 1] == LABEL_END) ? TRUE : FALSE;
}

Boolean isDataDirective(char* token){
	return (strcmp(token, DB_DIRECTIVE) == 0 || strcmp(token, DW_DIRECTIVE) == 0 || strcmp(token, DH_DIRECTIVE) == 0 || strcmp(token, ASCIZ_DIRECTIVE) == 0) ? TRUE : FALSE;
}

Boolean isExternDirective(char *token){
	return (strcmp(token, EXTERN_DIRECTIVE) == 0) ? TRUE : FALSE;
}

Boolean isEntryDirective(char *token){
	return (strcmp(token, ENTRY_DIRECTIVE) == 0) ? TRUE : FALSE;
}

Boolean isInstruction(char* token){
	return TRUE;
}

Boolean handleDataDirective(){

}

Boolean handleExternDirective(){

}

Boolean handleInstruction(){

}
