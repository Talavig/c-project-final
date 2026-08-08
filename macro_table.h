#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H

#include "consts.h"

/*
 * a struct to hold a single macro definition
 * name: a dynamically allocated string holding the name of the macro
 * content: a dynamically allocated string holding the content of the macro
 */
typedef struct {
	char *name;
	char *content;
} MacroTableEntry;


/*
 * a struct to hold a single item in the macro  table linked list.
 * macro_table_entry: the entry in the table
 * next_entry: a pointer to the next entry
 */
typedef struct MacroTableNode {
	MacroTableEntry macro_table_entry;
	struct MacroTableNode *next_entry;
} MacroTableNode;

/*
 * the macro table itself, which is a pointer to a linked list
 */
typedef MacroTableNode* MacroTable;

/*
 * search of a macro in the macro table by name (iterate over the linked list)
 * macro_table: the table to look in
 * name: the name of the macro to look for as specified in its name field
 *
 * return a pointer to the MacroTableNode if found, NULL if the macro does not exist
 */
MacroTableNode* findMacro(MacroTable macro_table, const char *name);

/*
 * add a new, empty entry to the start of the macro table
 * macro_table: a pointer to the macro table (pointer to the head node)
 * name: the name of the macro to be added
 *
 * return SUCCESS if the entry was added, FAILURE if addition failed
 */
Status addMacroToTable(MacroTable *macro_table, const char *name);

/*
 * append a new line of text to an existing macro's content. this is done using allocation/reallocation of the content memory.
 * macro_node: the macro to add the line to
 * line: the line to be added to the macros content
 *
 * return SUCCESS if the line was appended successfully, FAILURE on line addition issue
 */
Status addLineToMacro(MacroTableNode *macro_node, const char *line);

/*
 * frees all memory allocated for the symbol table by iterating over all of the tables nodes and set the main table pointer to null
 * macro_table: a pointer to the macro table to be freed.
 */
void freeMacroTable(MacroTable *macro_table);

#endif
