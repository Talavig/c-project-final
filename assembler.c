#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "messages.h"
#include "consts.h"
#include "utils.h"
#include "symbol_table.h"
#include "extern_table.h"

Status generate_output_files(char *file_base_name, SymbolTable *symbol_table, ExternTable *extern_table, int ic, int dc, unsigned char *data_image, unsigned long *code_image);

static Status createObjectFile(char *file_base_name, int ic, int dc, unsigned char *data_image, unsigned long *code_image);
static Status createEntriesFile(char *file_base_name, SymbolTable *symbol_table);
static Status createExternalsFile(char *file_base_name, ExternTable *extern_table);

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

Status generate_output_files(char *file_base_name, SymbolTable *symbol_table, ExternTable *extern_table, int ic, int dc, unsigned char *data_image, unsigned long *code_image) {
	Status files_creation_status = SUCCESS;

	if (createObjectFile(file_base_name, ic, dc, data_image, code_image) == FAILURE) {
		files_creation_status = FAILURE;
	}

	if (createEntriesFile(file_base_name, symbol_table) == FAILURE) {
		files_creation_status = FAILURE;
	}

	if (createExternalsFile(file_base_name, extern_table) == FAILURE) {
		files_creation_status = FAILURE;
	}

	return files_creation_status;
}


static Status createObjectFile(char *file_base_name, int ic, int dc, unsigned char *data_image, unsigned long *code_image){
	FILE *object_file;
	char *object_file_name = NULL;


	int current_address;
	int instruction_count;
	int data_count;
	int i;

	SymbolTableNode *current_symbol;
	ExternTableNode *current_extern;
	Boolean entry_created = FALSE;

	instruction_count = (ic - IC_INITIAL_VALUE) / INSTRUCTION_BYTES_SIZE;
	data_count = dc - DC_INITIAL_VALUE;

	object_file_name = createFileName(file_base_name, OUTPUT_OBJECT_FILE);
	if (ob_name == NULL) {
		return FAILURE;
	}

	object_file = fopen(object_file_name, "w");
	if (object_file == NULL) {
		fprintf(stderr, "Error: Cannot create object file %s\n", object_file_name);
		free(object_file_name);
		return FAILURE;
	}

	fprintf(object_file, "\t%d %d\n", ic - IC_INITIAL_VALUE, data_count);

	current_address = IC_INITIAL_VALUE;
	for (i = 0; i < instruction_count; i++) {
		fprintf(object_file, "%04d %08lX\n", current_address, (code_image[i] & 0xFFFFFFFF));
		current_address += INSTRUCTION_BYTES_SIZE;
	}

	current_address = ic;
	for (i = 0; i < data_count; i++) {
		fprintf(ob_file, "%04d %02X\n", current_address, (data_image[i] & 0xFF));
		current_address++;
	}

	fclose(object_file);
	free(object_file_name);
	return SUCCESS;
}

static Status createEntriesFile(char *file_base_name, SymbolTable *symbol_table){
	FILE *entries_file;
	char *entries_file_name;

	SymbolTableNode *current_symbol = *symbol_table;
	Boolean file_created = FALSE;

	while(current != NULL){
		if (current->entry.attributes & ENTRY) {
			if (!file_created) {
				entries_file_name = createFileName(file_base_name, OUTPUT_ENTRY_FILE);
				if (entries_file_name == NULL){
					return FAILURE;
				}
				entries_file = fopen(entries_file_name, "w");
				if (ent_file == NULL) {
					fprintf(stderr, "Error: Cannot create entry file %s\n", entries_file_name);
					free(entries_file_name);
					return FAILURE;
				}
				file_created = TRUE;
			}
			fprintf(entries_file, "%s %04d\n", current->entry.symbol, current->entry.value);
		}
		current = current->next;
	}

	if (file_created) {
		fclose(entries_file);
		free(entries_file_name);
	}

	return SUCCESS;
}

static Status createExternalsFile(char *file_base_name, ExternTable *extern_table){
	FILE *externals_file;
	char *externals_file_name;

	ExternTableNode *current_extern;

	if (*extern_table == NULL) {
		return SUCCESS;
	}

	externals_file_name = createFileName(file_base_name, OUTPUT_EXTERN_FILE);
	if (externals_file_name == NULL) {
		return FAILURE;
	}

	externals_file = fopen(externals_file_name, "w");
	if (externals_file == NULL) {
		fprintf(stderr, "Error: Cannot create externals file %s\n", object_file_name);
		free(externals_file_name);
		return FAILURE;
	}

	current = *extern_table;
	while (current != NULL) {
		fprintf(externals_file, "%s %04d\n", current->extern_table_entry.symbol, current->extern_table_entry.address);
		current = current->next_entry;
	}

	fclose(externals_file);
	free(externals_file_name);

	return SUCCESS;
}


