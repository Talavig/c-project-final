#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "symbol_table.h"


Status firstPass(char *file_base_name, SymbolTable* symbol_table, int* ic, int* dc, unsigned char *data_image, unsigned long* code_image, int *line_map);

#endif
