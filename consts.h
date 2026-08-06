/*
 * machine related consts
 */
#define NUM_REGISTERS 32
#define REGISTER_BIT_SIZE 32
#define MAX_MEMORY_SIZE (1 << 25)
#define IC_INITIAL_VALUE 100
#define DC_INITIAL_VALUE 0
#define INSTRUCTION_BYTES_SIZE 4

/*
 * file and assembler limits related consts
 */
#define MAX_LABEL_LENGTH 31
#define MAX_TOKEN_LENGTH 32 /* 31 for max label + 1 for \0*/
#define MAX_ASSEMBLY_LINE_COUNT 10000
#define MAX_SINGLE_LINE_LENGTH 80
#define MAX_OPERANDS_PER_LINE 40 /* derived from 80 - 3 chars for data decleration / 2 for comma and value*/


/*
 * file extentions
 */
#define INPUT_ASSEMBLY_FILE ".as"
#define MACRO_ASSEMBLY_FILE ".am"
#define OUTPUT_OBJECT_FILE ".ob"
#define OUTPUT_EXTERNALS_FILE ".ext"
#define OUTPUT_ENTRIES_FILE ".ent"

/*
 * max data types sizes in bytes
 */

#define DB_SIZE 1
#define DH_SIZE 2
#define DW_SIZE 4

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
#define DB_DIRECTIVE ".db"
#define DW_DIRECTIVE ".dw"
#define DH_DIRECTIVE ".dh"
#define ASCIZ_DIRECTIVE ".asciz"
#define ENTRY_DIRECTIVE "entry"
#define EXTERN_DIRECTIVE "extern"


/*
 * instruction opcodes and functs
 */

#define R_INSTRUCTIONS_ARITHMATIC_OPCODES 0
#define R_INSTRUCTIONS_MEMORY_OPCODES 1
#define NON_R_FUNCT_VALUE 0
#define NUM_INSTRUCTIONS 27
#define MAX_IMMED_VALUE (IMMED_MASK >> 1)
#define MIN_IMMED_VALUE (-MAX_IMMED_VALUE - 1)

typedef enum R_INSTRUCTIONS_ARITHMATIC_FUNCTS {
	ADD=1,
	SUB,
	AND,
	OR,
	NOR
} R_INSTRUCTIONS_ARITHMATIC_FUNCTS;

typedef enum R_INSTRUCTIONS_MEMORY_FUNCTS {
	MOVE=1,
	MVHI,
	MVLO
} R_INSTRUCTIONS_MEMORY_FUNCTS;


typedef enum I_INSTRUCTIONS_OPCODES {
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
} I_INSTRUCTIONS_OPCODES;

typedef enum J_INSTRUCTIONS_OPCODES {
	JMP=30,
	LA,
	CALL,
	HLT=63
} J_INSTRUCTIONS_OPCODES;


typedef enum {
	NONE = 0,
	CODE = 1,
	DATA = 2,
	ENTRY = 4,
	EXTERNAL = 8
} Attribute;

typedef enum {
	FALSE,
	TRUE
} Boolean;

typedef enum {
	FAILURE,
	SUCCESS
} Status;

/*
 * connsts for specific command types
 *
 */

typedef enum {
	R_TYPE,
	I_TYPE,
	J_TYPE
} InstructionType;

typedef struct {
	char *name,
	InstructionType type,
	int opcode,
	int oprands,
	int funct
} Instruction;


Instruction instructions[] = {
		{"add", R_TYPE, R_INSTRUCTIONS_ARITHMATIC_OPCODES, 3, ADD},
		{"sub", R_TYPE, R_INSTRUCTIONS_ARITHMATIC_OPCODES, 3, SUB},
		{"and", R_TYPE, R_INSTRUCTIONS_ARITHMATIC_OPCODES, 3, AND},
		{"or", R_TYPE, R_INSTRUCTIONS_ARITHMATIC_OPCODES, 3, OR},
		{"nor", R_TYPE, R_INSTRUCTIONS_ARITHMATIC_OPCODES, 3, NOR},

		{"move", R_TYPE, R_INSTRUCTIONS_MEMORY_OPCODES, 2, MOVE},
		{"mvhi", R_TYPE, R_INSTRUCTIONS_MEMORY_OPCODES, 2, MVHI},
		{"mvlo", R_TYPE, R_INSTRUCTIONS_MEMORY_OPCODES, 2, MVLO},

		{"addi", I_TYPE, ADDI, 3, NON_R_FUNCT_VALUE},
		{"subi", I_TYPE, SUBI, 3, NON_R_FUNCT_VALUE},
		{"andi", I_TYPE, ANDI, 3, NON_R_FUNCT_VALUE},
		{"ori", I_TYPE, ORI, 3, NON_R_FUNCT_VALUE},
		{"nori", I_TYPE, NORI, 3, NON_R_FUNCT_VALUE},

		{"bne", I_TYPE, BNE, 3, NON_R_FUNCT_VALUE},
		{"beq", I_TYPE, BEQ, 3, NON_R_FUNCT_VALUE},
		{"blt", I_TYPE, BLT, 3, NON_R_FUNCT_VALUE},
		{"bgt", I_TYPE, BGT, 3, NON_R_FUNCT_VALUE},

		{"lb", I_TYPE, LB, 3, NON_R_FUNCT_VALUE},
		{"sb", I_TYPE, SB, 3, NON_R_FUNCT_VALUE},
		{"lw", I_TYPE, LW, 3, NON_R_FUNCT_VALUE},
		{"sw", I_TYPE, SW, 3, NON_R_FUNCT_VALUE},
		{"lh", I_TYPE, LH, 3, NON_R_FUNCT_VALUE},
		{"sh", I_TYPE, SH, 3, NON_R_FUNCT_VALUE},

		{"jmp", J_TYPE, JMP, 1, NON_R_FUNCT_VALUE},
		{"la", J_TYPE, LA, 1, NON_R_FUNCT_VALUE},
		{"call", J_TYPE, CALL, 1, NON_R_FUNCT_VALUE},
		{"hlt", J_TYPE, HLT, 1, NON_R_FUNCT_VALUE}
};

/*
 * bitshift consts
 */

#define OPCODE_SHIFT 26
#define RS_SHIFT 21
#define RT_SHIFT 16
#define RD_SHIFT 11
#define FUNCT_SHIFT 6
#define REG_JUMP_SHIFT 25

#define BYTE_MASK 0xFF
#define IMMED_MASK 0xFFFF
#define ADDRESS_MASK 0x1FFFFFF

#define CODE_IMAGE_SIZE 4096
#define DATA_IMAGE_SIZE 4096
