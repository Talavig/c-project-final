#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extern_table.h"


ExternTable initializeExternTable() {
	return NULL;
}

Status addEntryToExternTable(ExternTable *extern_table, ExternTableEntry new_entry){
	ExternTableNode *new_node = (ExternTableNode*)malloc(sizeof(ExternTableNode));
	if (new_node == NULL) {
		printf(ERR_EXTERN_TABLE_MEMORY_ALLOCATION_FAILED);
		return FAILURE;
	}

	new_node->entry = new_entry;
	new_node->entry.symbol = (char*)malloc(strlen(new_entry.symbol) + 1);
	if (new_node->entry.symbol == NULL){
		printf(ERR_EXTERN_TABLE_STRING_MEMORY_ALLOCATION_FAILED);
		free(new_node);
		return FAILURE;
	}
	strcpy(new_node->entry.symbol, new_entry.symbol);

	new_node->next = *extern_table;
	*extern_table = new_node;
	return SUCCESS;
}

void freeExternTable(ExternTable *extern_table){
	ExternTableNode *current = extern_table;
	ExternTableNode *next_node;

	while (current != NULL){
		next_node = current->next;
		free(current->entry.symbol);
		free(current);
		current = next_node;
	}

	*extern_table = NULL;
}
