#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macro_table.h"
#include "consts.h"
#include "messages.h"


MacroTableEntry* findMacro(MacroTableEntry* start, const char* name){

	MacroTableEntry *current = start;
	while (current != NULL){
		if (strcmp(current->name, name) == 0) {
			return current;
		}
		current = current->next;
	}
	return NULL;
}

Status addMacroToTable(MacroTableEntry** start, const char* name){
	MacroTableEntry *new_macro = (MacroTableEntry*)malloc(sizeof(MacroTableEntry));

	if (new_macro == NULL) {
		fprintf(stderr, ERR_MEM_ALLOC_FAILED, "macro table entry");
		return FAILURE;
	}

	new_macro->name = (char*)malloc(strlen(name) + 1);
	if (new_macro->name == NULL) {
		fprintf(stderr, ERR_MEM_ALLOC_FAILED, "macro table entry's name");
		free(new_macro);
		return FAILURE;
	}
	else{
		strcpy(new_macro->name, name);
		new_macro->content = NULL;
		new_macro->next = *start;
		*start = new_macro;
		return SUCCESS;
	}
}

Status addLineToMacro(MacroTableEntry* macro, const char* line){
	char *temp_content;

	if (macro == NULL || line == NULL) {
		return FAILURE;
	}

	if (macro->content == NULL){
		macro->content = (char*)malloc(strlen(line) + 1);
		if (macro->content == NULL) {
			fprintf(stderr, ERR_MEM_ALLOC_FAILED, "macro entry content initilization");
			return FAILURE;
		}
		strcpy(macro->content, line);
	}
	else{
		temp_content = (char*)realloc(macro->content, strlen(macro->content) + strlen(line) + 1);
		if (temp_content == NULL){
			fprintf(stderr, ERR_MEM_ALLOC_FAILED, "macro entry content addition");
			return FAILURE;
		}
		macro->content = temp_content;
		strcat(macro->content, line);
	}
	return SUCCESS;
}

void freeMacroTable(MacroTableEntry* start){
	MacroTableEntry *current = start;
	MacroTableEntry *next_entry;

	while (current != NULL){
		next_entry = current->next;
		free(current->name);

		if (current->content != NULL) {
			free(current->content);
		}
		free(current);

		current = next_entry;
	}
}

Boolean checkMacroName(const char *name){
	int i;
	for (i = 0; i < NUM_INSTRUCTIONS; i++){
		if (strcmp(instructions[i].name, name) == 0){
			return TRUE;
		}
	}
	return FALSE;
}

Boolean checkMacroLine(const char *line){
	char word_a[MAX_SINGLE_LINE_LENGTH + 2] = {0};
	char word_b[MAX_SINGLE_LINE_LENGTH + 2] = {0};
	char word_c[MAX_SINGLE_LINE_LENGTH + 2] = {0};

	int parsed_words = sscanf(line, "%s %s %s", word_a, word_b, word_c);

	if (strcmp(word_a, MACRO_END) == 0){
		return (parsed_words == 1) ? TRUE : FALSE;
	}

	if (strcmp(word_a, MACRO_START) == 0){
		return (parsed_words == 2) ? TRUE : FALSE;
	}
	return TRUE;
}
