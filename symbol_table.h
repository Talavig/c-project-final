#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "consts.h"

typedef struct {
	char* symbol;
	int value;
	Attribute attributes;
}SymbolTableEntry;

typedef struct SymbolTableNode{
	SymbolTableEntry symbol_table_entry;
	struct SymbolTableNode* next_entry;
}SymbolTableNode;


typedef SymbolTableNode* SymbolTable;

SymbolTable initializeSymbolTable();

Status addEntryToSymbolTable(SymbolTable symbol_table, SymbolTableEntry new_entry);

void freeSymbolTable(SymbolTable* symbol_table);

SymbolTableNode* findSymbol(SymbolTable symbol_table, char *name);

#endif
