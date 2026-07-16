/*
 * machine related consts
 */
#define NUM_REGISTERS 32
#define REGISTER_BIT_SIZE 32
#define MAX_MEMORY_SIZE 2<<25
#define IC_INITIAL_VALUE 100
#define DC_INITIAL_VALUE 0
#define INSTRUCTION_BYTES_SIZE = 4

/*
 * file and assembler limits related consts
 */
#define MAX_LABEL_LENGTH 31
#define MAX_ASSEMBLY_LINE_COUNT 10000
#define MAX_SINGLE_LINE_LENGTH 80


/*
 * max data types sizes in bytes
 */

#define DB_SIZE 1
#define DH_SIZE 2
#define SIZE_DW 4

/*
 * special characters
 */

#define COMMENT ';'
#define LABEL_END ':'
#define DIRECTIVE_START '.'
#define STRING_WRAPPER '"'
#define END_OF_STRING '\0'
#define POSITIVE_NUMBER '+'
#define NEGATIVE_NUMBER '-'
#define REGISTER_INDICATOR '$'


/*
 * special strings
 */
#define MACRO_START "mcro"
#define MACRO_END "mcroend"
#define DB_DIRECTIVE "db"
#define DW_DIRECTIVE "dw"
#define DH_DIRECTIVE "dh"
#define ASCIZ_DIRECTIVE "asciz"
#define ENTRY_DIRECTIVE "entry"
#define EXTERN_DIRECTIVE "extern"

/*
 * instruction opcodes and functs
 */

#define R_INSTRUCTIONS_ARITHMATIC_OPCODES 0
#define R_INSTRUCTIONS_MEMORY_OPCODES 1

enum typedef R_INSTRUCTIONS_ARITHMATIC_FUNCTS {
	ADD=1,
	SUB,
	AND,
	OR,
	NOR
};

enum typedef R_INSTRUCTIONS_MEMORY_FUNCTS {
	MOVE=1,
	MVHI,
	MVLO
};


enum typedef I_INSTRUCTIONS_OPCODES {
	ADDI=10,
	SUBI,
	ANDI,
	ORI,
	NORI,
	BNE,
	BEQ,
	BLT,
	BGT,
	LB,
	SB,
	LW,
	SW,
	LH,
	SH
};

enum typedef J_INSTRUCTIONS_OPCODES {
	JMP=30,
	LA,
	CALL,
	HLT=63
};


typedef enum {
	CODE,
	DATA,
	ENTRY,
	EXTERNAL
} Attribute;

typedef enum {
	FALSE,
	TRUE
} Boolean;

typedef enum {
	FAILURE,
	SUCCESS
} Status;
