#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "messages.h"
#include "consts.h"
#include "utils.h"
#include "symbol_table.h"
#include "extern_table.h"

Status generate_output_files(char *file_base_name, SymbolTable *symbol_table, ExternTable *extern_table, int ic, int dc, unsigned char *data_image, unsigned long *code_image);


int main(int argc, char *argv[]){
	int ic;
	int dc;
	SymbolTable symbol_table;
	ExternTable extern_table;
	int i;
	unsigned long * code_image;
	unsigned char * data_image;
	char *file_base_name;


	if (argc < 2){
		fprintf(stderr, "Usage: %s <file1> <file2> ...\n", argv[0]);
		return EXIT_FAILURE;
	}



	for (i = 1; i < argc; i++){
		file_base_name = argv[i];
		ic = IC_INITIAL_VALUE;
		dc = DC_INITIAL_VALUE;
		code_image = (unsigned long *)calloc(MAX_MEMORY_SIZE / 4, sizeof(unsigned long));;
		data_image = (unsigned char *)calloc(MAX_MEMORY_SIZE, sizeof(unsigned char));
		SymbolTable symbol_table = NULL;
		ExternTable extern_table = NULL;

		if (code_image == NULL || data_image == NULL) {
			fprintf(stderr, "Error: Memory allocation failed for file %s.\n", file_base_name);
			if (code_image){
				free(code_image);
			}
			if (data_image){
				free(data_image);
			}
		}
		else{
			printf("Assembling %s...\n", file_base_name);

			if(preprocessScript(file_base_name) == FAILURE){
				printf("Errors found during the first pass of %s. Skipping to next file.\n", file_base_name);
			}
			else if (firstPass(file_base_name, &symbol_table, &ic, &dc, data_image, code_image) == FAILURE){
				printf("Errors found during the first pass of %s. Skipping to next file.\n", file_base_name);
			}
			else if (secondPass(file_base_name, symbol_table, ic, dc) == FAILURE){
				secondPass(file_base_name, &symbol_table, code_image, &extern_table) == FAILURE
			}
			else{
				if (generate_output_files(file_base_name, &symbol_table, &extern_table, ic, dc, data_image, code_image) == SUCCESS){
					printf("Successfully compiled %s!\n", file_base_name);
				}
				else{
					printf("Failed to generate output files for %s.\n", file_base_name);
				}
			}
			free(code_image);
			free(data_image);
			freeSymbolTable(&symbol_table);
			freeExternTable(&extern_table);
		}
	}
	return EXIT_SUCCESS;
}
