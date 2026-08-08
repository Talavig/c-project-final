#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_table.h"
#include "messages.h"


Status addEntryToSymbolTable(SymbolTable *symbol_table, SymbolTableEntry new_entry) {
	SymbolTableNode *new_node = (SymbolTableNode*)malloc(sizeof(SymbolTableNode));
	if (new_node == NULL) {
		fprintf(stderr, ERR_MEM_ALLOC_FAILED, "symbol table node");
		return FAILURE;
	}

	new_node->symbol_table_entry = new_entry;

	new_node->next_entry = *symbol_table;
	*symbol_table = new_node;
	return SUCCESS;
}

void freeSymbolTable(SymbolTable* symbol_table){
	SymbolTableNode *current = *symbol_table;
	SymbolTableNode *next_node;

	while (current != NULL){
		next_node = current->next_entry;
		free(current);
		current = next_node;
	}

	*symbol_table = NULL;
}

SymbolTableNode* findSymbol(SymbolTable symbol_table, char *symbol_name){
	SymbolTableNode *current = symbol_table;

	while (current != NULL) {
		if (strcmp(current->symbol_table_entry.symbol, symbol_name) == 0) {
			return current;
		}
		current = current->next_entry;
	}
	return NULL;
}
