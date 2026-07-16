#include "messages.h"
#include "consts.h"
#include "utils.h"
#include "symbol_table"




int main(int argc, char *argv[]){
	if (argc < 2){
		printf()
	}

	int ic = IC_INITIAL_VALUE;
	int dc = DC_INITIAL_VALUE;
	SymbolTable symbol_table;
	int i;

	for (i = 1; i< argc; i++){
		char *file_base_name = argv[i];

		if(preprocessScript(file_base_name) == FAILURE){

		}
		else if (firstPass(file_base_name, &symbol_table, &ic, &dc) == FAILURE){
			freeSymbolTable(symbol_table);
		}
		else if (secondPass(file_base_name, symbol_table, ic, dc) == FAILURE){
			freeSymbolTable(symbol_table);
		}
		else{
			generate_output_files(file_base_name, symbol_table, ic, dc);
		}
		freeSymbolTable(symbol_table);
	}






	return 0;
}
