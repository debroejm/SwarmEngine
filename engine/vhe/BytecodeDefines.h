#pragma once

// Precision Macros
#define CMD_PRECISION_1B        0b0000
#define CMD_PRECISION_2B        0b0001
#define CMD_PRECISION_4B        0b0010
#define CMD_PRECISION_8B        0b0011
#define CMD_PRECISION_1B_S2     0b0000
#define CMD_PRECISION_2B_S2     0b0100
#define CMD_PRECISION_4B_S2     0b1000
#define CMD_PRECISION_8B_S2     0b1100


// Jump Relative Flag
#define CMD_JUMP_RELATIVE       0b0100


// COMMAND : No Operation [NOP] : 00000000
/* DESCRIPTION:
 *   Performs no operation. Generally useless, but may be useful in certain situations.
 */
#define CMD_NOP                 0b00000000


// COMMAND : Halt [HALT] : 10000001
/* DESCRIPTION:
 *   Halts all operations and quits from running program.
 */
#define CMD_HALT                0b10000001


// COMMAND : Move to Register [MVTOREG] : 110000aa
/* DESCRIPTION:
 *   Moves data from memory to a register. Counterpart to MVTOMEM.
 *   Register to move to is specified by next byte in sequence.
 *   Address in memory to move from is specified another register, which is specified by the second byte in sequence.
 *   [aa] represents the byte width of the data to move:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_MVTOREG             0b11000000


// COMMAND : Move to Register Constant [MVTOREG_CONST] : 1110bbaa
/* DESCRIPTION:
 *   Moves data from memory to a register. Counterpart to MVTOMEM_CONST.
 *   Register to move to is specified by next byte in sequence.
 *   Address in memory to move from is specified by a constant, the width of which is specified by [bb]. The value of
 *   the constant is specified by the next bb bytes in sequence.
 *   [aa/bb] represents the byte width of the data to move:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_MVTOREG_CONST       0b11100000


// COMMAND : Move to Memory [MVTOMEM] : 110100aa
/* DESCRIPTION:
 *   Moves data from a register to memory. Counterpart to MVTOREG.
 *   Register to move from is specified by next byte in sequence.
 *   Address in memory to move to is specified another register, which is specified by the second byte in sequence.
 *   [aa] represents the byte width of the data to move:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_MVTOMEM             0b11010000


// COMMAND : Move to Memory Constant [MVTOMEM_CONST] : 1111bbaa
/* DESCRIPTION:
 *   Moves data from a register to memory. Counterpart to MVTOREG_CONST.
 *   Register to move from is specified by next byte in sequence.
 *   Address in memory to move to is specified by a constant, the width of which is specified by [bb]. The value of
 *   the constant is specified by the next bb bytes in sequence.
 *   [aa/bb] represents the byte width of the data to move:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_MVTOMEM_CONST       0b11110000


// COMMAND : Load Constant [LDCONST] : 111000aa
/* DESCRIPTION:
 *   Loads a constant value into a register.
 *   Register to move to is specified by the next byte in sequence.
 *   Number to load is specified by the following 1-8 bytes in sequence after register byte.
 *   [aa] represents the byte width of the data to move:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_LDCONST             0b11000100


// COMMAND : Copy Register [CPREG] : 11100100
/* DESCRIPTION:
 *   Copies the contents of one register to another.
 *   Register to copy from is specified by the next byte in sequence.
 *   Register to copy to is specified by the second byte in sequence.
 */
#define CMD_CPREG               0b11001000


// COMMAND : ALU Addition [ALU_ADD] : 01000000
/* DESCRIPTION:
 *   Performs the mathematical function of addition on two registers, and puts the output in a third.
 *   Registers to add are specified by the next two bytes, and register to output to is specified by the third.
 */
#define CMD_ALU_ADD             0b01000000


// COMMAND : ALU Constant Addition [ALU_ADD_CONST] : 011000aa
/* DESCRIPTION:
 *   Performs the mathematical function of addition on a register and a constant, and outputs to another register.
 *   Register to add and register to output to are specified by the next two bytes.
 *   Constant number to add is specified by the following 1-8 bytes in sequence after the first two.
 *   [aa] represents the byte width of the constant:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_ALU_ADD_CONST       0b01100000


// COMMAND : ALU Subtraction [ALU_SUB] : 01000001
/* DESCRIPTION:
 *   Performs the mathematical function of subtraction on two registers, and puts the output in a third.
 *   Registers to subtract are specified by the next two bytes, and register to output to is specified by the third.
 */
#define CMD_ALU_SUB             0b01000001


// COMMAND : ALU Constant Subtraction RHS [ALU_SUB_CONST_RHS] : 011001aa
/* DESCRIPTION:
 *   Performs the mathematical function of subtraction on a register and a constant, and outputs to another register.
 *   Register to subtract from and register to output to are specified by the next two bytes.
 *   Constant number is the right-hand side.
 *   Constant number is specified by the following 1-8 bytes in sequence after the first two.
 *   [aa] represents the byte width of the constant:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_ALU_SUB_CONST_RHS   0b01100100


// COMMAND : ALU Constant Subtraction LHS [ALU_SUB_CONST_LHS] : 011010aa
/* DESCRIPTION:
 *   Performs the mathematical function of subtraction on a register and a constant, and outputs to another register.
 *   Register to subtract from and register to output to are specified by the next two bytes.
 *   Constant number is the left-hand side.
 *   Constant number is specified by the following 1-8 bytes in sequence after the first two.
 *   [aa] represents the byte width of the constant:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_ALU_SUB_CONST_LHS   0b01101000


// COMMAND : ALU Multiplication [ALU_MULT] : 01000010
/* DESCRIPTION:
 *   Performs the mathematical function of multiplication on two registers, and puts the output in a third.
 *   Registers to multiply are specified by the next two bytes, and register to output to is specified by the third.
 */
#define CMD_ALU_MULT            0b01000010


// COMMAND : ALU Constant Multiplication [ALU_MULT_CONST] : 011011aa
/* DESCRIPTION:
 *   Performs the mathematical function of multiplication on a register and a constant, and outputs to another register.
 *   Register to multiply and register to output to are specified by the next two bytes.
 *   Constant number to multiply is specified by the following 1-8 bytes in sequence after the first two.
 *   [aa] represents the byte width of the constant:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_ALU_MULT_CONST       0b01101100


// COMMAND : ALU Division [ALU_DIV] : 01000011
/* DESCRIPTION:
 *   Performs the mathematical function of division on two registers, and puts the output in a third.
 *   Registers to divide are specified by the next two bytes, and register to output to is specified by the third.
 */
#define CMD_ALU_DIV             0b01000011


// COMMAND : ALU Constant Division RHS [ALU_DIV_CONST_RHS] : 011100aa
/* DESCRIPTION:
 *   Performs the mathematical function of division on a register and a constant, and outputs to another register.
 *   Register to divide and register to output to are specified by the next two bytes.
 *   Constant number is the right-hand side.
 *   Constant number is specified by the following 1-8 bytes in sequence after the first two.
 *   [aa] represents the byte width of the constant:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_ALU_DIV_CONST_RHS   0b01110000


// COMMAND : ALU Constant Division LHS [ALU_DIV_CONST_LHS] : 011101aa
/* DESCRIPTION:
 *   Performs the mathematical function of division on a register and a constant, and outputs to another register.
 *   Register to divide and register to output to are specified by the next two bytes.
 *   Constant number is the left-hand side.
 *   Constant number is specified by the following 1-8 bytes in sequence after the first two.
 *   [aa] represents the byte width of the constant:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_ALU_DIV_CONST_LHS   0b01110100


// COMMAND : ALU Modulus [ALU_MOD] : 01000100
/* DESCRIPTION:
 *   Performs the mathematical function of modulus on two registers, and puts the output in a third.
 *   Registers to modulo are specified by the next two bytes, and register to output to is specified by the third.
 */
#define CMD_ALU_MOD             0b01000100


// COMMAND : ALU Constant Modulus RHS [ALU_MOD_CONST_RHS] : 011110aa
/* DESCRIPTION:
 *   Performs the mathematical function of modulus on a register and a constant, and outputs to another register.
 *   Register to modulo and register to output to are specified by the next two bytes.
 *   Constant number is the right-hand side.
 *   Constant number is specified by the following 1-8 bytes in sequence after the first two.
 *   [aa] represents the byte width of the constant:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_ALU_MOD_CONST_RHS   0b01111000


// COMMAND : ALU Constant Modulus LHS [ALU_MOD_CONST_LHS] : 011111aa
/* DESCRIPTION:
 *   Performs the mathematical function of modulus on a register and a constant, and outputs to another register.
 *   Register to modulo and register to output to are specified by the next two bytes.
 *   Constant number is the left-hand side.
 *   Constant number is specified by the following 1-8 bytes in sequence after the first two.
 *   [aa] represents the byte width of the constant:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_ALU_MOD_CONST_LHS   0b01111100


// COMMAND : ALU Inversion [ALU_INV] : 01000101
/* DESCRIPTION:
 *   Applies two's complement inversion on a register. Operation happens in place.
 *   Register to invert is specified by the next byte in sequence.
 */
#define CMD_ALU_INV             0b01000101


// COMMAND : ALU Move Inversion [ALU_INV_MV] : 01010101
/* DESCRIPTION:
 *   Applies two's complement inversion on a register, and outputs to another register.
 *   Register to invert and register to output to are specified by the next two bytes in sequence.
 */
#define CMD_ALU_INV_MV          0b01010101


// COMMAND : ALU Increment [ALU_INC] : 01000110
/* DESCRIPTION:
 *   Increments the value in a register by one. Operation happens in place.
 *   Register to increment is specified by the next byte in sequence.
 */
#define CMD_ALU_INC             0b01000110


// COMMAND : ALU Move Increment [ALU_INC_MV] : 01010110
/* DESCRIPTION:
 *   Increments the value in a register by one, and outputs to another register.
 *   Register to increment and register to output to are specified by the next two bytes in sequence.
 */
#define CMD_ALU_INC_MV          0b01010110


// COMMAND : ALU Decrement [ALU_DEC] : 01000111
/* DESCRIPTION:
 *   Decrements the value in a register by one. Operation happens in place.
 *   Register to decrement is specified by the next byte in sequence.
 */
#define CMD_ALU_DEC             0b01000111


// COMMAND : ALU Move Decrement [ALU_DEC_MV] : 01010111
/* DESCRIPTION:
 *   Decrements the value in a register by one, and outputs to another register.
 *   Register to decrement and register to output to are specified by the next two bytes in sequence.
 */
#define CMD_ALU_DEC_MV          0b01010111


// COMMAND : Jump Always [JMP] : 00100abb
/* DESCRIPTION:
 *   Modifies the program counter according to a jump distance. Always jumps.
 *   [a] Relative flag. If this flag is set, the jump distance is relative to the current counter location, and can
 *     be negative. If it is not set, the jump distance is treated as unsigned.
 *   [bb] Represents the byte width of the jump distance:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_JMP                 0b00100000


// COMMAND : Jump Equal [JMP_EQL] : 00110abb
/* DESCRIPTION:
 *   Compares two registers, and if they are equal modifies the program counter according to a jump distance.
 *   Registers to compare are specified by the next two bytes in sequence.
 *   [a] Relative flag. If this flag is set, the jump distance is relative to the current counter location, and can
 *     be negative. If it is not set, the jump distance is treated as unsigned.
 *   [bb] Represents the byte width of the jump distance:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_JMP_EQL             0b00110000


// COMMAND : Jump Not Equal [JMP_NEQL] : 00111abb
/* DESCRIPTION:
 *   Compares two registers, and if they are not equal modifies the program counter according to a jump distance.
 *   Registers to compare are specified by the next two bytes in sequence.
 *   [a] Relative flag. If this flag is set, the jump distance is relative to the current counter location, and can
 *     be negative. If it is not set, the jump distance is treated as unsigned.
 *   [bb] Represents the byte width of the jump distance:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_JMP_NEQL            0b00111000


// COMMAND : Jump Less [JMP_LESS] : 00101abb
/* DESCRIPTION:
 *   Compares two registers, and if the first is less than the second modifies the program counter according to a
 *   jump distance.
 *   Registers to compare are specified by the next two bytes in sequence.
 *   [a] Relative flag. If this flag is set, the jump distance is relative to the current counter location, and can
 *     be negative. If it is not set, the jump distance is treated as unsigned.
 *   [bb] Represents the byte width of the jump distance:
 *     00: 1 byte
 *     01: 2 byte
 *     10: 4 byte
 *     11: 8 byte
 */
#define CMD_JMP_LESS            0b00101000