#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H

#include "consts.h"

typedef struct {
	char *name;
	char *content;
} MacroTableEntry;

typedef struct MacroTableNode {
	MacroTableEntry macro_table_entry;
	struct MacroTableNode *next_entry;
} MacroTableNode;

typedef MacroTableNode* MacroTable;

MacroTableNode* findMacro(MacroTable macro_table, const char *name);
Status addMacroToTable(MacroTable *macro_table, const char *name);
Status addLineToMacro(MacroTableNode *macro_node, const char *line);
void freeMacroTable(MacroTable *macro_table);










#endif
