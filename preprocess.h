#ifndef PREPROCESS_H
#define PREPROCESS_H

#include "consts.h"
#include "utils.h"

/*
 * the preprocessing stage of the assembler, generates an am file. this includes two important steps:
 * 1. macro expansion: look at the .as file, find macro definitions, and expand them
 * in the code wherever needed. this is done via using the macro table.
 * 2. line mapping: the preprocessing step generates a .am file, which is a run down version
 * of the .as file, in a easier to parse format for the first pass and all macros rolled out.
 * in that format, the lines from am need to map to their as counterparts, for logging errors.
 * this is done using the line_map.
 * file_base_name: the base name of the input file (without the .as extension)
 * line_map: an array populated by this function, mapping .am line numbers to .as line numbers
 *
 * SUCCESS if preprocessing completed without syntax errors or file issues, FAILURE otherwise.
 */
Status preprocessScript(char *file_base_name, int *line_map);

#endif
