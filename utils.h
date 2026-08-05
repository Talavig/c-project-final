#ifndef UTILS_H
#define UTILS_H

#include "consts.h"



char *skipWhitespaces(char *str);
Boolean getNextToken(char **src, char *dest);
Status extractOperands(char **line, char operands[][MAX_TOKEN_LENGTH], int *operand_count, int line_counter);
char *createFileName(char *base_name, char *extension);
Boolean isLineTooLong(const char *line, FILE *file);

Boolean isEmptyOrComment(char *current_line);
Boolean isLabelDef(char *token);
Boolean isDataDirective(char *token);
Boolean isExternDirective(char *token);
Boolean isEntryDirective(char *token);

Boolean isReservedWord(char *word);
Boolean isValidLabel(char *str);
Boolean parseRegister(const char *str, int *reg_num);
Boolean parseImmediate(const char *str, int *val);


Instruction *getInstruction(char *instruction_name);

#endif
