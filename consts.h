#ifndef CONSTS_H
#define CONSTS_H

/*
 * machine related consts
 */
#define NUM_REGISTERS 32 /* registeer count in imaginary machine */
#define REGISTER_BIT_SIZE 32 /* size of single register */
#define MAX_MEMORY_SIZE (1 << 25) /* size of smemory in machine */
#define IC_INITIAL_VALUE 100 /* starting value of the instruction counter as defined*/
#define DC_INITIAL_VALUE 0 /* starting value of the data counter as defined*/
#define INSTRUCTION_BYTES_SIZE 4 /* size in bytes of a single instrution*/

/*
 * file and assembler limits related consts
 */
#define MAX_LABEL_LENGTH 31
#define MAX_TOKEN_LENGTH 32 /* 31 for max label + 1 for \0*/
#define MAX_ASSEMBLY_LINE_COUNT 10000 /* assumpion: a single file may not pass 10000 lines*/
#define MAX_SINGLE_LINE_LENGTH 80 /*as defined by the manual*/
#define MAX_OPERANDS_PER_LINE (MAX_SINGLE_LINE_LENGTH / 2) /* derived from line len / 2 for comma and value*/


/*
 * file extentions
 */
#define INPUT_ASSEMBLY_FILE ".as"
#define FILE_EXTERNTION_LENGTH (sizeof(".as")/sizeof(char) - 1) /* comes down to 3, but written in this way for SOLID*/
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
#define STRING_WRAPPER '"'
#define END_OF_STRING '\0'
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
#define ENTRY_DIRECTIVE ".entry"
#define EXTERN_DIRECTIVE ".extern"

/*
 * bit masks % shift consts
 * all shift consts and masks are as described in the manual
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
#define MAX_IMMED_VALUE (IMMED_MASK >> 1)
#define MIN_IMMED_VALUE (-MAX_IMMED_VALUE - 1)

/*
 * output formatting consts
 */
#define BITS_IN_BYTE 8
#define BYTES_IN_OUTPUT_LINE 4
#define OUTPUT_HEADER_FORMAT "\t%d %d\n"
#define OUTPUT_ADDRESS_FORMAT "%04d"
#define OUTPUT_BYTE_FORMAT " %02X"
#define OUTPUT_SYMBOL_FORMAT "%s %04d\n"

/*
 * instruction opcodes and functs, all as described in the manual
 * these are implemented as enumes for later use in switch cases
 */

#define R_INSTRUCTIONS_ARITHMATIC_OPCODES 0
#define R_INSTRUCTIONS_MEMORY_OPCODES 1
#define NON_R_FUNCT_VALUE 0


/*
 * holds the FUNCT values for arithmatic r instructions that share an opcode of 0
 */
typedef enum R_INSTRUCTIONS_ARITHMATIC_FUNCTS {
	ADD=1,
	SUB,
	AND,
	OR,
	NOR
} R_INSTRUCTIONS_ARITHMATIC_FUNCTS;

/*
 * holds the FUNCT values for memory r instructions that share an opcode of 1
 */
typedef enum R_INSTRUCTIONS_MEMORY_FUNCTS {
	MOVE=1,
	MVHI,
	MVLO
} R_INSTRUCTIONS_MEMORY_FUNCTS;

/*
 * holds the OPCODE values for I instructions
 */
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

/*
 * holds the OPCODE values for J instructions
 */
typedef enum J_INSTRUCTIONS_OPCODES {
	JMP=30,
	LA,
	CALL,
	HLT=63
} J_INSTRUCTIONS_OPCODES;

/*
 * holds the different attributes each label can hase. the values are powers of 2,\
 * as we use a bitmap in order to hold th labels total abbributes state
 */
typedef enum {
	NONE = 0,
	CODE = 1,
	DATA = 2,
	ENTRY = 4,
	EXTERNAL = 8
} Attribute;

/*
 * holds the different instruction types values
 */
typedef enum {
	R_TYPE,
	I_TYPE,
	J_TYPE
} InstructionType;

/*
 * holds a single instruction:
 * name: a string containing the canon name of the instruction
 * type: the enum of the instruction type (r,i,j)
 * opcode: instruction's opcode
 * oprands: the number of oprands the instruction accepts
 * funct: used for r types, otherwise 0
 */
typedef struct {
	char *name;
	InstructionType type;
	int opcode;
	int oprands;
	int funct;
} Instruction;

/*
 * an array of akk instructions and its length, its actual value is computed in utils.c
 */
extern Instruction instructions[];
extern const int NUM_INSTRUCTIONS;

/*
 * control flow
 */

/*
 * an enum for boolean values
 */
typedef enum {
	FALSE,
	TRUE
} Boolean;

/*
 * an enum for the status of different processes happening in the assembly process
 */
typedef enum {
	FAILURE,
	SUCCESS
} Status;


#endif
