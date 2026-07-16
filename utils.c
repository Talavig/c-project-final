





void remove_blanks(char *str_input) {
	int blankless_string_index = 0; /* an index used to follow the spaceless substring */
	int i = 0;
	while (str_input[i] != STRING_TERMINATE)
	{
		if (!isspace(str_input[i])){
			str_input[blankless_string_index] = str_input[i];
			blankless_string_index++;
		}
		i++;
	}
	str_input[blankless_string_index] = '\0';
}
