#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_table.h"
#include "messages.h"


Status addEntryToSymbolTable(SymbolTable *symbol_table, SymbolTableEntry new_entry) {
	/* allocate memory for new node and check if it went smoothly*/
	SymbolTableNode *new_node = (SymbolTableNode*)malloc(sizeof(SymbolTableNode));
	if (new_node == NULL) {
		fprintf(stderr, ERR_MEM_ALLOC_FAILED, "symbol table node");
		return FAILURE;
	}

	/* copy the entry's data to the new node*/
	new_node->symbol_table_entry = new_entry;

	/* insert the node to the beginning of the list, and connect its start to the new node */
	new_node->next_entry = *symbol_table;
	*symbol_table = new_node;
	return SUCCESS;
}

void freeSymbolTable(SymbolTable* symbol_table){
	SymbolTableNode *current = *symbol_table; /* the current node we are freeing*/
	SymbolTableNode *next_node; /* the node after current*/

	/*iterate over all nodes in the table*/
	while (current != NULL){
		/* store the pointer to the next item, free the current one and go to the next one*/
		next_node = current->next_entry;
		free(current);
		current = next_node;
	}
	/* nullify the original table pointer*/
	*symbol_table = NULL;
}

SymbolTableNode* findSymbol(SymbolTable symbol_table, char *symbol_name){
	SymbolTableNode *current = symbol_table; /* a pointer to the current node looked in the table*/

	/* iterate over all items in the linked list */
	while (current != NULL) {
		/* if the current's node symbol is equal to the required symbol, return a pointer to it*/
		if (strcmp(current->symbol_table_entry.symbol, symbol_name) == 0) {
			return current;
		}
		current = current->next_entry;
	}
	/*otherwise, return a null pointer */
	return NULL;
}
