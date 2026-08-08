#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "extern_table.h"
#include "messages.h"


Status addEntryToExternTable(ExternTable *extern_table, ExternTableEntry new_entry){
	/* allocate memory for new node and check if it went smoothly*/
	ExternTableNode *new_node = (ExternTableNode*)malloc(sizeof(ExternTableNode));
	if (new_node == NULL) {
		fprintf(stderr, ERR_MEM_ALLOC_FAILED, "extern table node");
		return FAILURE;
	}

	/* copy the entry's data to the new node*/
	new_node->extern_table_entry = new_entry;

	/* insert the node to the beginning of the list, and connect its start to the new node */
	new_node->next_entry = *extern_table;
	*extern_table = new_node;
	return SUCCESS;
}

void freeExternTable(ExternTable *extern_table){
	ExternTableNode *current = *extern_table; /* the current node we are freeing*/
	ExternTableNode *next_node; /* the node after current*/

	/*iterate over all nodes in the table*/
	while (current != NULL){
		/* store the pointer to the next item, free the current one and go to the next one*/
		next_node = current->next_entry;
		free(current);
		current = next_node;
	}
	/* nullify the original table pointer*/
	*extern_table = NULL;
}
