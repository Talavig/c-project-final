#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H

#include "consts.h"

typedef struct MacroTableEntry{
	char* name;
	char *content;
	struct MacroTableEntry *next;
}MacroTableEntry;

MacroTableEntry* findMacro(MacroTableEntry* start, const char* name);
Status addMacroToTable(MacroTableEntry** start, const char* name);
Status addLineToMacro(MacroTableEntry* macro, const char* line);
void freeMacroTable(MacroTableEntry* start);
Boolean checkMacroName(const char *name);
Boolean checkMacroLine(const char *line);

#endif
