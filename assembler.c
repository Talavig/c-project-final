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

/* function prototypes for file generation */
static Status generate_output_files(const char *file_base_name, SymbolTable *symbol_table, ExternTable *extern_table, int ic, int dc, unsigned char *data_image, unsigned long *code_image);

static Status createObjectFile(const char *file_base_name, int ic, int dc, unsigned char *data_image, unsigned long *code_image);
static Status createEntriesFile(const char *file_base_name, SymbolTable *symbol_table);
static Status createExternalsFile(const char *file_base_name, ExternTable *extern_table);

/*
 * the main function of the assembler. handles the command line arguments, th multi pass assembly flow and the output files creation.
 */
int main(int argc, char *argv[]){
	int ic; /* instruction count, tracks code memory*/
	int dc;/* data count, tracks data memory*/
	SymbolTable symbol_table; /* a table storing labels values and attributes found in assembled file*/
	ExternTable extern_table; /* a table storing external refrences, as denoted by .extern*/
	int i; /* loop index*/
	unsigned long * code_image; /* dynamicly allocated array of the programs code memory image*/
	unsigned char * data_image;/* dynamicly allocated array of the programs data memory image*/
	int file_name_len; /* the length of the string holding the assembled file name*/
	char *file_base_name; /* filename of assembled file without a file extention*/
	int am_to_as_line[MAX_ASSEMBLY_LINE_COUNT + 1]; /* array used to map lines in the am file to those in the as fule for error location tracking*/
	char * macro_file_name; /*hold the .am file name */

	/* if assembler is called with no input files, exit*/
	if (argc < 2){
		fprintf(stderr, NO_ARGUMENTS_PASSED);
		return EXIT_FAILURE;
	}


	/* go over each input file argument*/
	for (i = 1; i < argc; i++){
		file_name_len = strlen(argv[i]);
		macro_file_name = NULL;

		/* if the file is not an .as file, ignore it*/
		if (file_name_len < FILE_EXTERNTION_LENGTH || strcmp(argv[i] + file_name_len - FILE_EXTERNTION_LENGTH, INPUT_ASSEMBLY_FILE) != 0) {
			fprintf(stderr, ERR_INVAILD_FILE_EXTENTION, argv[i], INPUT_ASSEMBLY_FILE);
		}
		else{
			/* allocate memory for file base name and copy its content*/
			file_base_name = (char *)malloc(file_name_len - FILE_EXTERNTION_LENGTH + 1);
			if (file_base_name == NULL) {
				fprintf(stderr, ERR_MEM_ALLOC_FAILED, "file name parsing");
			}
			else{
				strncpy(file_base_name, argv[i], file_name_len - FILE_EXTERNTION_LENGTH);
				file_base_name[file_name_len - FILE_EXTERNTION_LENGTH] = END_OF_STRING;

				macro_file_name = createFileName(file_base_name, MACRO_ASSEMBLY_FILE);
				if (macro_file_name == NULL) {
					free(file_base_name);
					continue;
				}

				/* initialize the ic/dc to their defined starting values, init the data structures for assembly based on the manual's reference*/
				ic = IC_INITIAL_VALUE;
				dc = DC_INITIAL_VALUE;
				code_image = (unsigned long *)calloc(MAX_MEMORY_SIZE / INSTRUCTION_BYTES_SIZE, sizeof(unsigned long));
				data_image = (unsigned char *)calloc(MAX_MEMORY_SIZE, sizeof(unsigned char));
				symbol_table = NULL;
				extern_table = NULL;

				/* check that dynamic memory allocations went smoothly*/
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
					/* run preprocessing step on file, which includes macro expantion and line mapping from am to as file*/
					if(preprocessScript(file_base_name , am_to_as_line) == FAILURE){
						printf(PREPROCESSING_ERRORS_FOUND, argv[i]);
					}
					/* run first pass on file, which handels symbol table construction, data, extern & partial instruction handling */
					else if (firstPass(macro_file_name, &symbol_table, &ic, &dc, data_image, code_image, am_to_as_line) == FAILURE){
						printf(FIRST_PASS_ERRORS_FOUND, argv[i]);
					}
					/* run second pass, which includes entry and remaining instructions handling*/
					else if (secondPass(macro_file_name, &symbol_table, code_image, &extern_table, am_to_as_line) == FAILURE) {
						printf(SECOND_PASS_ERRORS_FOUND, argv[i]);
					}
					else{
						/* if all assembly steps went smoothly, create the three output files*/
						if (generate_output_files(file_base_name, &symbol_table, &extern_table, ic, dc, data_image, code_image) == SUCCESS){
							printf(INFO_SUCCESSULLY_COMPILED, argv[i]);
						}
						else{
							printf(ERR_GENERATION_OUTPUT_FILES_FAILED, argv[i]);
						}
					}
					/* free dynamically allocated memory and data structuers for current file assembled*/
					free(code_image);
					free(data_image);
					freeSymbolTable(&symbol_table);
					freeExternTable(&extern_table);
				}
			}
			free(macro_file_name);
			free(file_base_name);
		}

	}
	printf(INFO_FINISHED_ASSEMBLING);
	return EXIT_SUCCESS;
}

/*
 * create the three output files for assembled file, .ob .ent and .ext
 * parameters:
 * file_base_name: the filename of the original .as file without the extention
 * symbol_table: pointer to the symbol table
 * extern_table: pointer to the externs usage table
 * ic: instruction counter value after finishing assembly
 * dc: data counter value after finishing assembly
 * data_image: pointer to the data memory image buffer
 * code_image: pointer to the code memory image buffer
 *
 *return SUCCESS if all files created successfully, FAILURE otherwise
 */
static Status generate_output_files(const char *file_base_name, SymbolTable *symbol_table, ExternTable *extern_table, int ic, int dc, unsigned char *data_image, unsigned long *code_image) {
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

/*
 * create the object file .ob, which holds the instruction and data image of the assembled code, in the required encoding.
 *
 * file_base_name the filename of the original .as file without the extention
 * ic: instruction counter value after finishing assembly
 * dc: data counter value after finishing assembly
 * data_image: pointer to the data memory image buffer
 * code_image: pointer to the code memory image buffer
 *
 * return SUCCESS on success, FAILURE on file creation or writing error.
 */
static Status createObjectFile(const char *file_base_name, int ic, int dc, unsigned char *data_image, unsigned long *code_image){
	FILE *object_file;
	char *object_file_name = NULL;

	int current_address; /* the address we need to put n the .ob file now*/
	int instruction_count; /* how many items appear in the instruction image*/
	int data_count; /* how many items appear in the data image*/
	int i, j; /* loop iterators */
	int bytes_to_copy; /* counts how many byres are needed to be copied for the current line*/

	/* get the instruction_count by subtraction the initial value and dividing by a single instruction size
	 * get the data_count by subtracting the initial value (no need for division as each item is a byte)*/
	instruction_count = (ic - IC_INITIAL_VALUE) / INSTRUCTION_BYTES_SIZE;
	data_count = dc - DC_INITIAL_VALUE;

	/*
	 * create the object filename and file
	 */
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

	/* add tot he object file the header which holds the byte count for each section, code and data*/
	fprintf(object_file, OUTPUT_HEADER_FORMAT, ic - IC_INITIAL_VALUE, data_count);

	/* write instruction memory image (4 bytes per line in Little-Endian format) */
	current_address = IC_INITIAL_VALUE;
	for (i = 0; i < instruction_count; i++) {
		fprintf(object_file, OUTPUT_ADDRESS_FORMAT, current_address);
		for (j = 0; j < BYTES_IN_OUTPUT_LINE; j++) {
			/* bytes are gathered using a moving mask over each 4 byte instruction*/
			fprintf(object_file, OUTPUT_BYTE_FORMAT, (unsigned int)((code_image[i] >> (j * BITS_IN_BYTE)) & BYTE_MASK));
		}
		fprintf(object_file, "\n");
		current_address += INSTRUCTION_BYTES_SIZE;
	}

	/* write data memory image (up to 4 bytes per line) */
	current_address = ic;
	for (i = 0; i < data_count; i += BYTES_IN_OUTPUT_LINE) {
		/* if left data does not divide by 4, we print the bytes_to_copy to make sure to stay in format*/
		bytes_to_copy = (data_count - i < BYTES_IN_OUTPUT_LINE) ? (data_count - i) : BYTES_IN_OUTPUT_LINE;
		fprintf(object_file, OUTPUT_ADDRESS_FORMAT, current_address);
		for (j = 0; j < bytes_to_copy; j++) {
			/*we copy byte by byte, each line holds 4 bytes so we move to the ith row and print the jth byte */
			fprintf(object_file, OUTPUT_BYTE_FORMAT, data_image[i + j]);
		}
		fprintf(object_file, "\n");
		current_address += BYTES_IN_OUTPUT_LINE;
	}
	 /*close files and free memory	  */
	fclose(object_file);
	free(object_file_name);
	return SUCCESS;
}

/*
 * create the entries file .ent, which holds all of the symbols with the entry attributes with their addresses.
 *
 * file_base_name the filename of the original .as file without the extention
 * symbol_table: a pointer to the symbol table
 *
 * return SUCCESS on success, FAILURE on file creation or writing error.
 */
static Status createEntriesFile(const char *file_base_name, SymbolTable *symbol_table){
	FILE *entries_file;
	char *entries_file_name;

	SymbolTableNode *current_symbol = *symbol_table; /* a pointer used to iterate the symbol table list*/
	Boolean file_created = FALSE; /* a boolean used to track if the file was already created or not*/

	while(current_symbol != NULL){
		/* entries file must not be empty, so we create it only if a symbol with the entry attribute is found*/
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
			/* add to file the symbol in the correct format*/
			fprintf(entries_file, OUTPUT_SYMBOL_FORMAT, current_symbol->symbol_table_entry.symbol, current_symbol->symbol_table_entry.value);
		}
		/* iterate over the linked list*/
		current_symbol = current_symbol->next_entry;
	}

	if (file_created) {
		fclose(entries_file);
		free(entries_file_name);
	}

	return SUCCESS;
}

/*
 * create the entries file .ext, which holds all of the symbols with the extern attributes with their addresses.
 *
 * file_base_name the filename of the original .as file without the extention
 * extern_table: a pointer to the extern table
 *
 * return SUCCESS on success, FAILURE on file creation or writing error.
 */
static Status createExternalsFile(const char *file_base_name, ExternTable *extern_table){
	FILE *externals_file;
	char *externals_file_name;

	ExternTableNode *current_extern; /* a pointer used to iterate the extern table list*/

	/* dont create the .ext file if there are no .extern symbols in the file*/
	if (*extern_table == NULL) {
		return SUCCESS;
	}

	/*create .ext file name and file*/
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

	/* iterate over the extern table and put its content in the correct format in the file*/
	current_extern = *extern_table;
	while (current_extern != NULL) {
		fprintf(externals_file, OUTPUT_SYMBOL_FORMAT, current_extern->extern_table_entry.symbol, current_extern->extern_table_entry.address);
		current_extern = current_extern->next_entry;
	}

	/*close files and free memory*/
	fclose(externals_file);
	free(externals_file_name);

	return SUCCESS;
}


