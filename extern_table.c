#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "extern_table.h"
#include "messages.h"

ExternTable initializeExternTable() {
	return NULL;
}

Status addEntryToExternTable(ExternTable *extern_table, ExternTableEntry new_entry){
	ExternTableNode *new_node = (ExternTableNode*)malloc(sizeof(ExternTableNode));
	if (new_node == NULL) {
		printf(ERR_EXTERN_TABLE_MEMORY_ALLOCATION_FAILED);
		return FAILURE;
	}

	new_node->extern_table_entry = new_entry;
	new_node->next_entry = *extern_table;
	*extern_table = new_node;
	return SUCCESS;
}

void freeExternTable(ExternTable *extern_table){
	ExternTableNode *current = *extern_table;
	ExternTableNode *next_node;

	while (current != NULL){
		next_node = current->next_entry;
		free(current);
		current = next_node;
	}

	*extern_table = NULL;
}
