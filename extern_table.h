#ifndef EXT_TABLE_H
#define EXT_TABLE_H

#include "consts.h"



typedef struct ExternTableEntry {
	char symbol[MAX_LABEL_LENGTH];
	int address;
} ExternTableEntry;

typedef struct ExternTableNode{
	ExternTableEntry extern_table_entry;
	struct ExternTableNode* next_entry;
} ExternTableNode;

typedef ExternTableNode* ExternTable;


ExternList initializeExternList();
Status addEntryToExternTable(ExternTable *extern_table, ExternTableEntry new_entry);
void freeExternTable(ExternTable *extern_table);


#endif
