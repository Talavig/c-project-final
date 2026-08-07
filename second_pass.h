#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "symbol_table.h"


Status secondPass(char *file_base_name, SymbolTable *symbol_table, unsigned long *code_image, ExternTable *extern_table);


#endif
