#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macro_table.h"
#include "consts.h"
#include "messages.h"

MacroTableNode* findMacro(MacroTable macro_table, const char *name) {
	MacroTableNode *current = macro_table; /* a pointer to the current node in the table*/

	/*iterate over the linked list and return a node where a name match is found */
	while (current != NULL) {
		if (strcmp(current->macro_table_entry.name, name) == 0) {
			return current;
		}
		current = current->next_entry;
	}
	/*otherwise, return null*/
	return NULL;
}

Status addMacroToTable(MacroTable *macro_table, const char *name) {
	MacroTableNode *new_node = (MacroTableNode*)malloc(sizeof(MacroTableNode)); /* the new node to be added*/

	/*check if memory allocation went smoothly	 */
	if (new_node == NULL) {
		fprintf(stderr, ERR_MEM_ALLOC_FAILED, "macro table node");
		return FAILURE;
	}

	/*allocate a string for the macro's name and check if it went well */
	new_node->macro_table_entry.name = (char*)malloc(strlen(name) + 1);
	if (new_node->macro_table_entry.name == NULL) {
		fprintf(stderr, ERR_MEM_ALLOC_FAILED, "macro table entry name");
		free(new_node);
		return FAILURE;
	}

	/* copy the name to the new node and set its content to null*/
	strcpy(new_node->macro_table_entry.name, name);
	new_node->macro_table_entry.content = NULL;

	/* set the new node's next entry to be the original list head, and move the head to the new node*/
	new_node->next_entry = *macro_table;
	*macro_table = new_node;
	return SUCCESS;
}

Status addLineToMacro(MacroTableNode *macro_node, const char *line) {
	char *temp_content; /* pointer for content allocation safety*/

	/* if one of the pointers is null, break out*/
	if (macro_node == NULL || line == NULL) {
		return FAILURE;
	}

	/* case 1: the content is empty, so we malloc a new buffer for it*/
	if (macro_node->macro_table_entry.content == NULL) {
		/* allocate the content's size +1 for a null terminator*/
		macro_node->macro_table_entry.content = (char*)malloc(strlen(line) + 1);
		if (macro_node->macro_table_entry.content == NULL) {
			fprintf(stderr, ERR_MEM_ALLOC_FAILED, "macro entry content initialization");
			return FAILURE;
		}
		/* copy content to buffer*/
		strcpy(macro_node->macro_table_entry.content, line);
	}
	/* case 2: the content hase some lines, and we realloc it in order to add to it*/
	else{
		/* reallocate memory to the size of the previous content + the new content + null terminator*/
		temp_content = (char*)realloc(macro_node->macro_table_entry.content, strlen(macro_node->macro_table_entry.content) + strlen(line) + 1);
		if (temp_content == NULL) {
			fprintf(stderr, ERR_MEM_ALLOC_FAILED, "macro entry content addition");
			return FAILURE;
		}
		/* concatenate content to buffer*/
		macro_node->macro_table_entry.content = temp_content;
		strcat(macro_node->macro_table_entry.content, line);
	}
	return SUCCESS;
}

void freeMacroTable(MacroTable *macro_table) {
	MacroTableNode *current = *macro_table; /* the current node we are freeing*/
	MacroTableNode *next_node; /* a pointer to the next node in the linked list*/

	/* iterate over all the nodes*/
	while (current != NULL) {
		/* store the pointer to the next item, free the current one and go to the next one*/
		/* if there is a dynamically allocated content buffer, free it too*/
		next_node = current->next_entry;
		free(current->macro_table_entry.name);
		if (current->macro_table_entry.content != NULL) {
			free(current->macro_table_entry.content);
		}
		free(current);

		current = next_node;
	}
	/* nullify the original table pointer*/
	*macro_table = NULL;
}




