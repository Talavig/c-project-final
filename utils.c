#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "consts.h"



char *skipWhitespaces(char *str) {
	if (str == NULL) return NULL;

	while (*str != '\0' && isspace((unsigned char)*str)) {
		str++;
	}
	return str;
}


Boolean getNextToken(char **src, char *dest) {
	int i = 0;

	*src = skipWhitespaces(*src);
	if (**src == '\0') {
		dest[0] = '\0';
		return FALSE;
	}

	while (**src != '\0' && !isspace((unsigned char)**src) && **src != ','){
		dest[i] = **src;
		(*src)++;
		i++;
	}

	dest[i] = '\0';
	return TRUE;
}
