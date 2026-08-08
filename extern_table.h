#ifndef EXTERN_TABLE_H
#define EXTERN_TABLE_H

#include "consts.h"


/*
 * a struct to hold a single entry (label) in the assembler's extern table.
 * symbol: a label (up to a label's length), as it appears in the asm code
 * address: the address assigned to the label
 */
typedef struct ExternTableEntry {
	char symbol[MAX_LABEL_LENGTH];
	int address;
} ExternTableEntry;

/*
 * a struct to hold a single item in the extern table linked list.
 * extern_table_entry: the entry in the table
 * next_entry: a pointer to the next entry
 */
typedef struct ExternTableNode{
	ExternTableEntry extern_table_entry;
	struct ExternTableNode* next_entry;
} ExternTableNode;

/*
 * the exterm table itself, which is a pointer to a linked list
 */
typedef ExternTableNode* ExternTable;

/*
 * add a new extern entry to the start of the extern table
 * extern_table: a pointer to the extern table (pointer to the head node)
 * new_entry: the extern data to be added
 *
 * return SUCCESS if the entry was added, FAILURE if addition failed
 */
Status addEntryToExternTable(ExternTable *extern_table, ExternTableEntry new_entry);

/*
 * frees all memory allocated for the extern table by iterating over all of the tables nodes and set the main table pointer to null
 * extern_table: a pointer to the extern table to be freed.
 */
void freeExternTable(ExternTable *extern_table);


#endif
