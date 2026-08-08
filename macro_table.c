#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macro_table.h"
#include "consts.h"
#include "messages.h"

MacroTableNode* findMacro(MacroTable macro_table, const char *name) {
	MacroTableNode *current = macro_table;
	while (current != NULL) {
		if (strcmp(current->macro_table_entry.name, name) == 0) {
			return current;
		}
		current = current->next_entry;
	}
	return NULL;
}

Status addMacroToTable(MacroTable *macro_table, const char *name) {
	MacroTableNode *new_node = (MacroTableNode*)malloc(sizeof(MacroTableNode));
	if (new_node == NULL) {
		fprintf(stderr, ERR_MEM_ALLOC_FAILED, "macro table node");
		return FAILURE;
	}

	new_node->macro_table_entry.name = (char*)malloc(strlen(name) + 1);
	if (new_node->macro_table_entry.name == NULL) {
		fprintf(stderr, ERR_MEM_ALLOC_FAILED, "macro table entry name");
		free(new_node);
		return FAILURE;
	}

	strcpy(new_node->macro_table_entry.name, name);
	new_node->macro_table_entry.content = NULL;
	new_node->next_entry = *macro_table;
	*macro_table = new_node;
	return SUCCESS;
}

Status addLineToMacro(MacroTableNode *macro_node, const char *line) {
	char *temp_content;
	if (macro_node == NULL || line == NULL) {
		return FAILURE;
	}

	if (macro_node->macro_table_entry.content == NULL) {
		macro_node->macro_table_entry.content = (char*)malloc(strlen(line) + 1);
		if (macro_node->macro_table_entry.content == NULL) {
			fprintf(stderr, ERR_MEM_ALLOC_FAILED, "macro entry content initialization");
			return FAILURE;
		}
		strcpy(macro_node->macro_table_entry.content, line);
	}
	else{
		temp_content = (char*)realloc(macro_node->macro_table_entry.content, strlen(macro_node->macro_table_entry.content) + strlen(line) + 1);
		if (temp_content == NULL) {
			fprintf(stderr, ERR_MEM_ALLOC_FAILED, "macro entry content addition");
			return FAILURE;
		}
		macro_node->macro_table_entry.content = temp_content;
		strcat(macro_node->macro_table_entry.content, line);
	}
	return SUCCESS;
}

void freeMacroTable(MacroTable *macro_table) {
	MacroTableNode *current = *macro_table;
	MacroTableNode *next_node;

	while (current != NULL) {
		next_node = current->next_entry;
		free(current->macro_table_entry.name);
		if (current->macro_table_entry.content != NULL) {
			free(current->macro_table_entry.content);
		}
		free(current);

		current = next_node;
	}
	*macro_table = NULL;
}




