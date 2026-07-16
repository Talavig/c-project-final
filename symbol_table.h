#include "consts.h"

typedef struct {
	char* symbol,
	int value,
	Attribute attribute
}SymbolTableEntry;

typedef struct {
	SymbolTableEntry symbol_table_entry,
	SymbolTableEntry * next_entry
}SymbolTable;


SymbolTable* initializeSymbolTable();

void addEntryToSymbolTable(SymbolTable symbol_table, SymbolTableEntry entry);

void freeSymbolTable(SymbolTable* symbol_table);
