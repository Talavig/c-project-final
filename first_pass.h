#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "symbol_table.h"

/*
 * execute the first pass of the assembler. this pass is responsible for the following:
 * 1.validate line syntax (line length, directive and instructions are in format)
 * 2.find and save the symbols to the symbol table, and make sure to update data symbols to start their address at the ICF
 * 3.handle data directives and encode them to machine code
 * 4.handle extern directives by adding them to the symbol table
 * 5.handle part of the instructions (all r types, non branch I types, partial J support) and encode them to machine code
 * file_base_name: the base name of the source file
 * symbol_table: pointer to the symbol table (linked list)
 * ic: pointer to the instruction counter
 * dc: pointer to the data counter
 * data_image: array representing the data memory segment
 * code_image: srray representing the code memory segment (instructions)
 * line_map: array mapping .am line numbers back to their original .as line numbers
 *
 * return SUCCESS if the first pass completed without any errors, FAILURE otherwise
 */
Status firstPass(char *file_base_name, SymbolTable* symbol_table, int* ic, int* dc, unsigned char *data_image, unsigned long* code_image, int *line_map);

#endif
