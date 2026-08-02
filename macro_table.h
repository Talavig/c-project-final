#include "consts.h"

typedef struct MacroTableEntry{
	char* name;
	char *content;
	struct * MacroTableEntry next;
}MacroTableEntry;

MacroTableEntry* findMacro(MacroTableEntry* start, const char* name);
void addMacroToTable(MacroTableEntry** start, const char* name);
void addLineToMacro(MacroTableEntry* macro, const char* line);
void freeMacroTable(MacroTableEntry* start);
Boolean checkMacroName(const char *name);
Boolean checkMacroLine(const char *line);

