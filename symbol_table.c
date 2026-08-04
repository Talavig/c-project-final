#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"


SymbolTable initializeSymbolTable() {
    return NULL;
}

Status addEntryToSymbolTable(SymbolTable *symbol_table, SymbolTableEntry new_entry) {
	SymbolTableNode *new_node = (SymbolTableNode*)malloc(sizeof(SymbolTableNode));
	if (new_node == NULL) {
		printf(ERR_SYMBOL_TABLE_MEMORY_ALLOCATION_FAILED);
		return FAILURE;
	}

	new_node->entry = new_entry;
	new_node->entry.symbol = (char*)malloc(strlen(new_entry.symbol) + 1);
	if (new_node->entry.symbol == NULL){
		printf(ERR_SYMBOL_TABLE_STRING_MEMORY_ALLOCATION_FAILED);
		free(new_node);
		return FAILURE;
	}
	strcpy(new_node->entry.symbol, new_entry.symbol);

	new_node->next = *symbol_table;
	*symbol_table = new_node;
	return SUCCESS;
}

void freeSymbolTable(SymbolTable* symbol_table){
	SymbolTableNode *current = symbol_table;
	SymbolTableNode *next_node;

	while (current != NULL){
		next_node = current->next;
		free(current->entry.symbol);
		free(current);
		current = next_node;
	}
}

SymbolTableNode* findSymbol(SymbolTable symbol_table, char *name){
	SymbolTableNode *current = symbol_table;

	while (current != NULL) {
		if (strcmp(current->entry.symbol, symbol_name) == 0) {
			return current;
		}
		current = current->next;
	}
	return NULL;
}
