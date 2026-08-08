#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "consts.h"

/*
 * a struct to hold a single entry (label) in the assembler's symbol table.
 * symbol: a label (up to a label's length), as it appears in the asm code
 * value: the address assigned to the label
 * attributes: a bitmask representing the labels attributes
 */
typedef struct SymbolTableEntry{
	char symbol[MAX_LABEL_LENGTH];
	int value;
	Attribute attributes;
}SymbolTableEntry;


/*
 * a struct to hold a single item in the symbol table linked list.
 * symbol_table_entry: the entry in the table
 * next_entry: a pointer to the next entry
 */
typedef struct SymbolTableNode{
	SymbolTableEntry symbol_table_entry;
	struct SymbolTableNode* next_entry;
}SymbolTableNode;


/*
 * the symbol table itself, which is a pointer to a linked list
 */
typedef SymbolTableNode* SymbolTable;

/*
 * add a new symbol entry to the start of the symbol table
 * symbol_table: a pointer to the symbol table (pointer to the head node)
 * new_entry: the symbol data to be added
 *
 * return SUCCESS if the entry was added, FAILURE if addition failed
 */
Status addEntryToSymbolTable(SymbolTable *symbol_table, SymbolTableEntry new_entry);


/*
 * frees all memory allocated for the symbol table by iterating over all of the tables nodes and set the main table pointer to null
 * symbol_table: a pointer to the symbol table to be freed.
 */
void freeSymbolTable(SymbolTable* symbol_table);

/*
 * search of a symbol in the symbol table by name (iterate over the linked list)
 * symbol_table: the table to look in
 * name: the name of the symbol to look for as specified in its symbol field
 *
 * return a pointer to the SymbolTableNode if found, NULL if the symbol does not exist
 */
SymbolTableNode* findSymbol(SymbolTable symbol_table, char *symbol_name);

#endif
