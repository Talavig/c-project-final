#include "consts.h"



char *skipWhitespaces(char *str);
Boolean getNextToken(char **src, char *dest);


Status extractOperands(char **ptr, char operands[][MAX_TOKEN_LENGTH], int *operand_count);
