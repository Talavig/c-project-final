#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "consts.h"
#include "symbol_table.h"
#include "extern_table.h"

/*
 * executes the second pass of the assembler on the expanded macro file.
 * this pass does the following:
 * 1. resolve .entry directives and mark the corresponding symbols in the symbol table.
 * 2. resolve operand addresses for branching (I-type) and jumping (J-type) instructions.
 * 3. calculate relative memory offsets for conditional branches.
 * 4. record the memory addresses where external symbols are used into the extern table.
 *
 * file_base_name: the base name of the source file.
 * symbol_table: pointer to the fully populated symbol table from the first pass
 * code_image: the code image, partially filled at the first pass
 * extern_table: pointer to the externals table
 * line_map: array mapping .am line numbers back to their original .as line numbers
 *
 * return SUCCESS if the second pass completed without missing labels or conflicts, FAILURE otherwise
 */
Status secondPass(char *file_base_name, SymbolTable *symbol_table, unsigned long *code_image, ExternTable *extern_table, int *line_map);

#endif
