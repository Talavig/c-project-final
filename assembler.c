#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "messages.h"
#include "consts.h"
#include "utils.h"
#include "symbol_table.h"
#include "extern_table.h"
#include "preprocess.h"
#include "first_pass.h"
#include "second_pass.h"

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
	int file_name_len;
	char *file_base_name;
	int am_to_as_line[MAX_ASSEMBLY_LINE_COUNT + 1];


	if (argc < 2){
		fprintf(stderr, NO_ARGUMENTS_PASSED);
		return EXIT_FAILURE;
	}



	for (i = 1; i < argc; i++){
		file_name_len = strlen(argv[i]);
		if (file_name_len < 3 || strcmp(argv[i] + file_name_len - 3, INPUT_ASSEMBLY_FILE) != 0) {
			fprintf(stderr, ERR_INVAILD_FILE_EXTENTION, argv[i], INPUT_ASSEMBLY_FILE);
		}
		else{
			file_base_name = (char *)malloc(file_name_len - 2);
			if (file_base_name == NULL) {
				fprintf(stderr, ERR_MEM_ALLOC_FAILED, "file name parsing");
			}
			else{
				strncpy(file_base_name, argv[i], file_name_len - 3);
				file_base_name[file_name_len - 3] = '\0';
				ic = IC_INITIAL_VALUE;
				dc = DC_INITIAL_VALUE;
				code_image = (unsigned long *)calloc(MAX_MEMORY_SIZE / 4, sizeof(unsigned long));
				data_image = (unsigned char *)calloc(MAX_MEMORY_SIZE, sizeof(unsigned char));
				symbol_table = NULL;
				extern_table = NULL;

				if (code_image == NULL || data_image == NULL) {
					fprintf(stderr, ERR_MEM_ALLOC_FAILED, "code and data images");
					if (code_image){
						free(code_image);
					}
					if (data_image){
						free(data_image);
					}
				}
				else{
					printf(INFO_ASSEMBLING, argv[i]);
					if(preprocessScript(file_base_name , am_to_as_line) == FAILURE){
						printf(PREPROCESSING_ERRORS_FOUND, argv[i]);
					}
					else if (firstPass(file_base_name, &symbol_table, &ic, &dc, data_image, code_image, am_to_as_line) == FAILURE){
						printf(FIRST_PASS_ERRORS_FOUND, argv[i]);
					}
					else if (secondPass(file_base_name, &symbol_table, code_image, &extern_table, am_to_as_line) == FAILURE) {
						printf(SECOND_PASS_ERRORS_FOUND, argv[i]);
					}
					else{
						if (generate_output_files(file_base_name, &symbol_table, &extern_table, ic, dc, data_image, code_image) == SUCCESS){
							printf(INFO_SUCCESSULLY_COMPILED, argv[i]);
						}
						else{
							printf(ERR_GENERATION_OUTPUT_FILES_FAILED, argv[i]);
						}
					}
					free(code_image);
					free(data_image);
					freeSymbolTable(&symbol_table);
					freeExternTable(&extern_table);
				}
			}
			free(file_base_name);

		}

	}
	printf(INFO_FINISHED_ASSEMBLING);
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
	int i, j;
	int bytes_to_copy;

	instruction_count = (ic - IC_INITIAL_VALUE) / INSTRUCTION_BYTES_SIZE;
	data_count = dc - DC_INITIAL_VALUE;

	object_file_name = createFileName(file_base_name, OUTPUT_OBJECT_FILE);
	if (object_file_name == NULL) {
		return FAILURE;
	}

	object_file = fopen(object_file_name, "w");
	if (object_file == NULL) {
		fprintf(stderr, ERR_FILE_OPERATION_FAILED, "create", "object", object_file_name);
		free(object_file_name);
		return FAILURE;
	}

	fprintf(object_file, OUTPUT_HEADER_FORMAT, ic - IC_INITIAL_VALUE, data_count);

	current_address = IC_INITIAL_VALUE;
	for (i = 0; i < instruction_count; i++) {
		fprintf(object_file, OUTPUT_ADDRESS_FORMAT, current_address);
		for (j = 0; j < 4; j++) {
			fprintf(object_file, OUTPUT_BYTE_FORMAT, (unsigned int)((code_image[i] >> (j * BITS_IN_BYTE)) & BYTE_MASK));
		}
		fprintf(object_file, "\n");
		current_address += INSTRUCTION_BYTES_SIZE;
	}

	current_address = ic;
	for (i = 0; i < data_count; i += BYTES_IN_OUTPUT_LINE) {
		bytes_to_copy = (data_count - i < BYTES_IN_OUTPUT_LINE) ? (data_count - i) : BYTES_IN_OUTPUT_LINE;
		fprintf(object_file, OUTPUT_ADDRESS_FORMAT, current_address);
		for (j = 0; j < bytes_to_copy; j++) {
			fprintf(object_file, OUTPUT_BYTE_FORMAT, data_image[i + j]);
		}
		fprintf(object_file, "\n");
		current_address += BYTES_IN_OUTPUT_LINE;
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

	while(current_symbol != NULL){
		if (current_symbol->symbol_table_entry.attributes & ENTRY) {
			if (!file_created) {
				entries_file_name = createFileName(file_base_name, OUTPUT_ENTRIES_FILE);
				if (entries_file_name == NULL){
					return FAILURE;
				}
				entries_file = fopen(entries_file_name, "w");
				if (entries_file == NULL) {
					fprintf(stderr, ERR_FILE_OPERATION_FAILED, "create", "entries", entries_file_name);
					free(entries_file_name);
					return FAILURE;
				}
				file_created = TRUE;
			}
			fprintf(entries_file, OUTPUT_SYMBOL_FORMAT, current_symbol->symbol_table_entry.symbol, current_symbol->symbol_table_entry.value);
		}
		current_symbol = current_symbol->next_entry;
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

	externals_file_name = createFileName(file_base_name, OUTPUT_EXTERNALS_FILE);
	if (externals_file_name == NULL) {
		return FAILURE;
	}

	externals_file = fopen(externals_file_name, "w");
	if (externals_file == NULL) {
		fprintf(stderr, ERR_FILE_OPERATION_FAILED, "create", "externals", externals_file_name);
		free(externals_file_name);
		return FAILURE;
	}

	current_extern = *extern_table;
	while (current_extern != NULL) {
		fprintf(externals_file, OUTPUT_SYMBOL_FORMAT, current_extern->extern_table_entry.symbol, current_extern->extern_table_entry.address);
		current_extern = current_extern->next_entry;
	}

	fclose(externals_file);
	free(externals_file_name);

	return SUCCESS;
}


