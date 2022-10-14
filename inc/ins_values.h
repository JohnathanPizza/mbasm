/*
 * this file contains macros for the opcodes of all instructions and related enums
 * the list is organized by chunks of instructions by abbreviated name, and then ordered by lowest to highest opcode value
 * the list of instruction addressing mode shorthands are below:
 *
 * ABS absolute
 * ZP zero page
 * ABSII absolute indexed indirect
 * ZPII zero page indexed indirect
 * ABSX absoulte indexed X
 * ZPX zero page indexed X
 * ABSY absolute indexed Y
 * ZPY zero page indexed Y
 * ABSI absolute indirect
 * ZPI zero page indirect
 * IM immediate
 * R program counter relative
 * ZPIIY zero page indirect indexed with Y
 * ACC accumulator
 * S stack
 * I implied
 *
 * refer to the manual for more information
 */

#ifndef INSTRUCTION_VALUES_H
#define INSTRUCTION_VALUES_H

/*
 * symbols for important data
 *
 * insNameStrings is an array holding the string literals for the abbreviated names of each instruction
 * at each index, the value of the identifier of each InstructionName enum is the index for which the string literal matches the enum identifier
 * for example: insNameStrings[IN_LDX] = "LDX"
 * this array is used to determine which instruction to use from text in source code
 *
 * opcodes is a 2D array of all the opcode values
 * each entry on the first level is a table of 16 values that can fit in 1 byte
 * each table contains either 0's or valid opcode values per addressing mode for each instruction
 * since there are a total of 16 different instruction addressing modes, each table has 16 values
 * when determing the opcode to use, the first index is an InstructionName enum and the second index is an AddressingMode enum
 * the combination yields the opcode for an instruction from its typed name and determined addressing mode
 * the values are either 0 or nonzero opcodes, and an entry of 0 means that addressing mode is not valid for that instruction
 *
 * flagsList is an array of arrays of 3 characters and represents the combinations of characters the user can input or be determined that form an addressing mode
 * each instruction addressing mode can be determined by
 *	1) if the value is zero-page sized, not zero-page sized, or has no value
 *	2) if indirect, immediate, or indexed addressing is used
 *	3) and an extra specifier for indirect or indexed addressing
 * if the values being compared match the values at an index N, then the Nth addressing mode enum is the correct one to use
 */

extern const char* insNameStrings[];
extern const short opcodes[][16];
extern const char flagsList[][3];

#define OPC_ADC_ZPII	0x61
#define OPC_ADC_ZP	0x65
#define OPC_ADC_IM	0x69
#define OPC_ADC_ABS	0x6D
#define OPC_ADC_ZPIIY	0x71
#define OPC_ADC_ZPI	0x72
#define OPC_ADC_ZPX	0x75
#define OPC_ADC_ABSY	0x79
#define OPC_ADC_ABSX	0x7D

#define OPC_AND_ZPII	0x21
#define OPC_AND_ZP	0x25
#define OPC_AND_IM	0x29
#define OPC_AND_ABS	0x2D
#define OPC_AND_ZPIIY	0x31
#define OPC_AND_ZPI	0x32
#define OPC_AND_ZPX	0x35
#define OPC_AND_ABSY	0x39
#define OPC_AND_ABSX	0x3D

#define OPC_ASL_ZP	0x06
#define OPC_ASL_ACC	0x0A
#define OPC_ASL_ABS	0x0E
#define OPC_ASL_ZPX	0x16
#define OPC_ASL_ABSX	0x1E

#define OPC_BBR0_R	0x0F
#define OPC_BBR1_R	0x1F
#define OPC_BBR2_R	0x2F
#define OPC_BBR3_R	0x3F
#define OPC_BBR4_R	0x4F
#define OPC_BBR5_R	0x5F
#define OPC_BBR6_R	0x6F
#define OPC_BBR7_R	0x7F

#define OPC_BBS0_R	0x8F
#define OPC_BBS1_R	0x9F
#define OPC_BBS2_R	0xAF
#define OPC_BBS3_R	0xBF
#define OPC_BBS4_R	0xCF
#define OPC_BBS5_R	0xDF
#define OPC_BBS6_R	0xEF
#define OPC_BBS7_R	0xFF

#define OPC_BCC_R	0x90

#define OPC_BCS_R	0xB0

#define OPC_BEQ_R	0xF0

#define OPC_BIT_ZP	0x24
#define OPC_BIT_ABS	0x2C
#define OPC_BIT_ZPX	0x34
#define OPC_BIT_ABSX	0x3C
#define OPC_BIT_IM	0x89

#define OPC_BMI_R	0x30

#define OPC_BNE_R	0xD0

#define OPC_BPL_R	0x10

#define OPC_BRA_R	0x80

#define OPC_BRK_S	0x00

#define OPC_BVC_R	0x50

#define OPC_BVS_R	0x70

#define OPC_CLC_I	0x18

#define OPC_CLD_I	0xD8

#define OPC_CLI_I	0x58

#define OPC_CLV_I	0xB8

#define OPC_CMP_ZPII	0xC1
#define OPC_CMP_ZP	0xC5
#define OPC_CMP_IM	0xC9
#define OPC_CMP_ABS	0xCD
#define OPC_CMP_ZPIIY	0xD1
#define OPC_CMP_ZPI	0xD2
#define OPC_CMP_ZPX	0xD5
#define OPC_CMP_ABSY	0xD9
#define OPC_CMP_ABSX	0xDD

#define OPC_CPX_IM	0xE0
#define OPC_CPX_ZP	0xE4
#define OPC_CPX_ABS	0xEC

#define OPC_CPY_IM	0xC0
#define OPC_CPY_ZP	0xC4
#define OPC_CPY_ABS	0xCC

#define OPC_DEC_ACC	0x3A
#define OPC_DEC_ZP	0xC6
#define OPC_DEC_ABS	0xCE
#define OPC_DEC_ZPX	0xD6
#define OPC_DEC_ABSX	0xDE

#define OPC_DEX_I	0xCA

#define OPC_DEY_I	0x88

#define OPC_EOR_ZPII	0x41
#define OPC_EOR_ZP	0x45
#define OPC_EOR_IM	0x49
#define OPC_EOR_ABS	0x4D
#define OPC_EOR_ZPIIY	0x51
#define OPC_EOR_ZPI	0x52
#define OPC_EOR_ZPX	0x55
#define OPC_EOR_ABSY	0x59
#define OPC_EOR_ABSX	0x5D

#define OPC_INC_ACC	0x1A
#define OPC_INC_ZP	0xE6
#define OPC_INC_ABS	0xEE
#define OPC_INC_ZPX	0xF6
#define OPC_INC_ABSX	0xFE

#define OPC_INX_I	0xE8

#define OPC_INY_I	0xC8

#define OPC_JMP_ABS	0x4C
#define OPC_JMP_ABSI	0x6C
#define OPC_JMP_ABSII	0x7C

#define OPC_JSR_ABS	0x20

#define OPC_LDA_ZPII	0xA1
#define OPC_LDA_ZP	0xA5
#define OPC_LDA_IM	0xA9
#define OPC_LDA_ABS	0xAD
#define OPC_LDA_ZPIIY	0xB1
#define OPC_LDA_ZPI	0xB2
#define OPC_LDA_ZPX	0xB5
#define OPC_LDA_ABSY	0xB9
#define OPC_LDA_ABSX	0xBD

#define OPC_LDX_IM	0xA2
#define OPC_LDX_ZP	0xA6
#define OPC_LDX_ABS	0xAE
#define OPC_LDX_ZPY	0xB6
#define OPC_LDX_ABSY	0xBE

#define OPC_LDY_IM	0xA0
#define OPC_LDY_ZP	0xA4
#define OPC_LDY_ABS	0xAC
#define OPC_LDY_ZPX	0xB4
#define OPC_LDY_ABSX	0xBC

#define OPC_LSR_ZP	0x46
#define OPC_LSR_ACC	0x4A
#define OPC_LSR_ABS	0x4E
#define OPC_LSR_ZPX	0x56
#define OPC_LSR_ABSX	0x5E

#define OPC_NOP_I	0xEA

#define OPC_ORA_ZPII	0x01
#define OPC_ORA_ZP	0x05
#define OPC_ORA_IM	0x09
#define OPC_ORA_ABS	0x0D
#define OPC_ORA_ZPIIY	0x11
#define OPC_ORA_ZPI	0x12
#define OPC_ORA_ZPX	0x15
#define OPC_ORA_ABSY	0x19
#define OPC_ORA_ABSX	0x1D

#define OPC_PHA_S	0x48

#define OPC_PHP_S	0x08

#define OPC_PHX_S	0xDA

#define OPC_PHY_S	0x5A

#define OPC_PLA_S	0x68

#define OPC_PLP_S	0x28

#define OPC_PLX_S	0xFA

#define OPC_PLY_S	0x7A

#define OPC_RMB0_ZP	0x07
#define OPC_RMB1_ZP	0x17
#define OPC_RMB2_ZP	0x27
#define OPC_RMB3_ZP	0x37
#define OPC_RMB4_ZP	0x47
#define OPC_RMB5_ZP	0x57
#define OPC_RMB6_ZP	0x67
#define OPC_RMB7_ZP	0x77

#define OPC_ROL_ZP	0x26
#define OPC_ROL_ACC	0x2A
#define OPC_ROL_ABS	0x2E
#define OPC_ROL_ZPX	0x36
#define OPC_ROL_ABSX	0x3E

#define OPC_ROR_ZP	0x66
#define OPC_ROR_ACC	0x6A
#define OPC_ROR_ABS	0x6E
#define OPC_ROR_ZPX	0x76
#define OPC_ROR_ABSX	0x7E

#define OPC_RTI_S	0x40

#define OPC_RTS_S	0x60

#define OPC_SBC_ZPII	0xE1
#define OPC_SBC_ZP	0xE5
#define OPC_SBC_IM	0xE9
#define OPC_SBC_ABS	0xED
#define OPC_SBC_ZPIIY	0xF1
#define OPC_SBC_ZPI	0xF2
#define OPC_SBC_ZPX	0xF5
#define OPC_SBC_ABSY	0xF9
#define OPC_SBC_ABSX	0xFD

#define OPC_SEC_I	0x38

#define OPC_SED_I	0xF8

#define OPC_SEI_I	0x78

#define OPC_SMB0_ZP	0x87
#define OPC_SMB1_ZP	0x97
#define OPC_SMB2_ZP	0xA7
#define OPC_SMB3_ZP	0xB7
#define OPC_SMB4_ZP	0xC7
#define OPC_SMB5_ZP	0xD7
#define OPC_SMB6_ZP	0xE7
#define OPC_SMB7_ZP	0xF7

#define OPC_STA_ZPII	0x81
#define OPC_STA_ZP	0x85
#define OPC_STA_ABS	0x8D
#define OPC_STA_ZPIIY	0x91
#define OPC_STA_ZPI	0x92
#define OPC_STA_ZPX	0x95
#define OPC_STA_ABSY	0x99
#define OPC_STA_ABSX	0x9D

#define OPC_STP_I	0xDB

#define OPC_STX_ZP	0x86
#define OPC_STX_ABS	0x8E
#define OPC_STX_ZPY	0x96

#define OPC_STY_ZP	0x84
#define OPC_STY_ABS	0x8C
#define OPC_STY_ZPX	0x94

#define OPC_STZ_ZP	0x64
#define OPC_STZ_ZPX	0x74
#define OPC_STZ_ABS	0x9C
#define OPC_STZ_ABSX	0x9E

#define OPC_TAX_I	0xAA

#define OPC_TAY_I	0xA8

#define OPC_TRB_ZP	0x14
#define OPC_TRB_ABS	0x1C

#define OPC_TSB_ZP	0x04
#define OPC_TSB_ABS	0x0C

#define OPC_TSX_I	0xBA

#define OPC_TXA_I	0x8A

#define OPC_TXS_I	0x9A

#define OPC_TYA_I	0x98

#define OPC_WAI_I	0xCB

// unique value for each abbreviated instruction name

enum InstructionName{
	IN_ADC,
	IN_AND,
	IN_ASL,
	IN_BBR0,
	IN_BBR1,
	IN_BBR2,
	IN_BBR3,
	IN_BBR4,
	IN_BBR5,
	IN_BBR6,
	IN_BBR7,
	IN_BBS0,
	IN_BBS1,
	IN_BBS2,
	IN_BBS3,
	IN_BBS4,
	IN_BBS5,
	IN_BBS6,
	IN_BBS7,
	IN_BCC,
	IN_BCS,
	IN_BEQ,
	IN_BIT,
	IN_BMI,
	IN_BNE,
	IN_BPL,
	IN_BRA,
	IN_BRK,
	IN_BVC,
	IN_BVS,
	IN_CLC,
	IN_CLD,
	IN_CLI,
	IN_CLV,
	IN_CMP,
	IN_CPX,
	IN_CPY,
	IN_DEC,
	IN_DEX,
	IN_DEY,
	IN_EOR,
	IN_INC,
	IN_INX,
	IN_INY,
	IN_JMP,
	IN_JSR,
	IN_LDA,
	IN_LDX,
	IN_LDY,
	IN_LSR,
	IN_NOP,
	IN_ORA,
	IN_PHA,
	IN_PHP,
	IN_PHX,
	IN_PHY,
	IN_PLA,
	IN_PLP,
	IN_PLX,
	IN_PLY,
	IN_RMB0,
	IN_RMB1,
	IN_RMB2,
	IN_RMB3,
	IN_RMB4,
	IN_RMB5,
	IN_RMB6,
	IN_RMB7,
	IN_ROL,
	IN_ROR,
	IN_RTI,
	IN_RTS,
	IN_SBC,
	IN_SEC,
	IN_SED,
	IN_SEI,
	IN_SMB0,
	IN_SMB1,
	IN_SMB2,
	IN_SMB3,
	IN_SMB4,
	IN_SMB5,
	IN_SMB6,
	IN_SMB7,
	IN_STA,
	IN_STP,
	IN_STX,
	IN_STY,
	IN_STZ,
	IN_TAX,
	IN_TAY,
	IN_TRB,
	IN_TSB,
	IN_TSX,
	IN_TXA,
	IN_TXS,
	IN_TYA,
	IN_WAI,
	IN_NULL
};

// unique value for each addressing mode

enum AddressingMode{
	AM_ABS,
	AM_ABSII,
	AM_ABSX,
	AM_ABSY,
	AM_ABSI,
	AM_ACC,
	AM_IM,
	AM_I,
	AM_PCR,
	AM_S,
	AM_ZP,
	AM_ZPII,
	AM_ZPX,
	AM_ZPY,
	AM_ZPI,
	AM_ZPIIY,
	AM_NULL
};

#endif
