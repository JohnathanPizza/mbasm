#include "ins_values.h"
#include <stddef.h>

#define INS(n) [IN_ ## n] = #n

const char* insNameStrings[] = {
	INS(ADC),
	INS(AND),
	INS(ASL),
	INS(BBR0),
	INS(BBR1),
	INS(BBR2),
	INS(BBR3),
	INS(BBR4),
	INS(BBR5),
	INS(BBR6),
	INS(BBR7),
	INS(BBS0),
	INS(BBS1),
	INS(BBS2),
	INS(BBS3),
	INS(BBS4),
	INS(BBS5),
	INS(BBS6),
	INS(BBS7),
	INS(BCC),
	INS(BCS),
	INS(BEQ),
	INS(BIT),
	INS(BMI),
	INS(BNE),
	INS(BPL),
	INS(BRA),
	INS(BRK),
	INS(BVC),
	INS(BVS),
	INS(CLC),
	INS(CLD),
	INS(CLI),
	INS(CLV),
	INS(CMP),
	INS(CPX),
	INS(CPY),
	INS(DEC),
	INS(DEX),
	INS(DEY),
	INS(EOR),
	INS(INC),
	INS(INX),
	INS(INY),
	INS(JMP),
	INS(JSR),
	INS(LDA),
	INS(LDX),
	INS(LDY),
	INS(LSR),
	INS(NOP),
	INS(ORA),
	INS(PHA),
	INS(PHP),
	INS(PHX),
	INS(PHY),
	INS(PLA),
	INS(PLP),
	INS(PLX),
	INS(PLY),
	INS(RMB0),
	INS(RMB1),
	INS(RMB2),
	INS(RMB3),
	INS(RMB4),
	INS(RMB5),
	INS(RMB6),
	INS(RMB7),
	INS(ROL),
	INS(ROR),
	INS(RTI),
	INS(RTS),
	INS(SBC),
	INS(SEC),
	INS(SED),
	INS(SEI),
	INS(SMB0),
	INS(SMB1),
	INS(SMB2),
	INS(SMB3),
	INS(SMB4),
	INS(SMB5),
	INS(SMB6),
	INS(SMB7),
	INS(STA),
	INS(STP),
	INS(STX),
	INS(STY),
	INS(STZ),
	INS(TAX),
	INS(TAY),
	INS(TRB),
	INS(TSB),
	INS(TSX),
	INS(TXA),
	INS(TXS),
	INS(TYA),
	INS(WAI),
	NULL
};

const short opcodes[][16] = {
	[IN_ADC] = {
		[AM_ABS] = OPC_ADC_ABS + 1,
		[AM_ABSX] = OPC_ADC_ABSX + 1,
		[AM_ABSY] = OPC_ADC_ABSY + 1,
		[AM_IM] = OPC_ADC_IM + 1,
		[AM_ZP] = OPC_ADC_ZP + 1,
		[AM_ZPI] = OPC_ADC_ZPI + 1,
		[AM_ZPII] = OPC_ADC_ZPII + 1,
		[AM_ZPIIY] = OPC_ADC_ZPIIY + 1,
		[AM_ZPX] = OPC_ADC_ZPX + 1,
	},
	[IN_AND] = {
		[AM_ABS] = OPC_AND_ABS + 1,
		[AM_ABSX] = OPC_AND_ABSX + 1,
		[AM_ABSY] = OPC_AND_ABSY + 1,
		[AM_IM] = OPC_AND_IM + 1,
		[AM_ZP] = OPC_AND_ZP + 1,
		[AM_ZPI] = OPC_AND_ZPI + 1,
		[AM_ZPII] = OPC_AND_ZPII + 1,
		[AM_ZPIIY] = OPC_AND_ZPIIY + 1,
		[AM_ZPX] = OPC_AND_ZPX + 1,
	},
	[IN_ASL] = {
		[AM_ABS] = OPC_ASL_ABS + 1,
		[AM_ABSX] = OPC_ASL_ABSX + 1,
		[AM_ACC] = OPC_ASL_ACC + 1,
		[AM_ZP] = OPC_ASL_ZP + 1,
		[AM_ZPX] = OPC_ASL_ZPX + 1,
	},
	[IN_BBR0] = {
		[AM_PCR] = OPC_BBR0_R + 1,
	},
	[IN_BBR1] = {
		[AM_PCR] = OPC_BBR1_R + 1,
	},
	[IN_BBR2] = {
		[AM_PCR] = OPC_BBR2_R + 1,
	},
	[IN_BBR3] = {
		[AM_PCR] = OPC_BBR3_R + 1,
	},
	[IN_BBR4] = {
		[AM_PCR] = OPC_BBR4_R + 1,
	},
	[IN_BBR5] = {
		[AM_PCR] = OPC_BBR5_R + 1,
	},
	[IN_BBR6] = {
		[AM_PCR] = OPC_BBR6_R + 1,
	},
	[IN_BBR7] = {
		[AM_PCR] = OPC_BBR7_R + 1,
	},
	[IN_BBS0] = {
		[AM_PCR] = OPC_BBS0_R + 1,
	},
	[IN_BBS1] = {
		[AM_PCR] = OPC_BBS1_R + 1,
	},
	[IN_BBS2] = {
		[AM_PCR] = OPC_BBS2_R + 1,
	},
	[IN_BBS3] = {
		[AM_PCR] = OPC_BBS3_R + 1,
	},
	[IN_BBS4] = {
		[AM_PCR] = OPC_BBS4_R + 1,
	},
	[IN_BBS5] = {
		[AM_PCR] = OPC_BBS5_R + 1,
	},
	[IN_BBS6] = {
		[AM_PCR] = OPC_BBS6_R + 1,
	},
	[IN_BBS7] = {
		[AM_PCR] = OPC_BBS7_R + 1,
	},
	[IN_BCC] = {
		[AM_PCR] = OPC_BCC_R + 1,
	},
	[IN_BCS] = {
		[AM_PCR] = OPC_BCS_R + 1,
	},
	[IN_BEQ] = {
		[AM_PCR] = OPC_BEQ_R + 1,
	},
	[IN_BIT] = {
		[AM_ABS] = OPC_BIT_ABS + 1,
		[AM_ABSX] = OPC_BIT_ABSX + 1,
		[AM_IM] = OPC_BIT_IM + 1,
		[AM_ZP] = OPC_BIT_ZP + 1,
		[AM_ZPX] = OPC_BIT_ZPX + 1,
	},
	[IN_BMI] = {
		[AM_PCR] = OPC_BMI_R + 1,
	},
	[IN_BNE] = {
		[AM_PCR] = OPC_BNE_R + 1,
	},
	[IN_BPL] = {
		[AM_PCR] = OPC_BPL_R + 1,
	},
	[IN_BRA] = {
		[AM_PCR] = OPC_BRA_R + 1,
	},
	[IN_BRK] = {
		[AM_S] = OPC_BRK_S + 1,
	},
	[IN_BVC] = {
		[AM_PCR] = OPC_BVC_R + 1,
	},
	[IN_BVS] = {
		[AM_PCR] = OPC_BVS_R + 1,
	},
	[IN_CLC] = {
		[AM_I] = OPC_CLC_I + 1,
	},
	[IN_CLD] = {
		[AM_I] = OPC_CLD_I + 1,
	},
	[IN_CLI] = {
		[AM_I] = OPC_CLI_I + 1,
	},
	[IN_CLV] = {
		[AM_I] = OPC_CLV_I + 1,
	},
	[IN_CMP] = {
		[AM_ABS] = OPC_CMP_ABS + 1,
		[AM_ABSX] = OPC_CMP_ABSX + 1,
		[AM_ABSY] = OPC_CMP_ABSY + 1,
		[AM_IM] = OPC_CMP_IM + 1,
		[AM_ZP] = OPC_CMP_ZP + 1,
		[AM_ZPI] = OPC_CMP_ZPI + 1,
		[AM_ZPII] = OPC_CMP_ZPII + 1,
		[AM_ZPIIY] = OPC_CMP_ZPIIY + 1,
		[AM_ZPX] = OPC_CMP_ZPX + 1,
	},
	[IN_CPX] = {
		[AM_ABS] = OPC_CPX_ABS + 1,
		[AM_IM] = OPC_CPX_IM + 1,
		[AM_ZP] = OPC_CPX_ZP + 1,
	},
	[IN_CPY] = {
		[AM_ABS] = OPC_CPY_ABS + 1,
		[AM_IM] = OPC_CPY_IM + 1,
		[AM_ZP] = OPC_CPY_ZP + 1,
	},
	[IN_DEC] = {
		[AM_ABS] = OPC_DEC_ABS + 1,
		[AM_ABSX] = OPC_DEC_ABSX + 1,
		[AM_ACC] = OPC_DEC_ACC + 1,
		[AM_ZP] = OPC_DEC_ZP + 1,
		[AM_ZPX] = OPC_DEC_ZPX + 1,
	},
	[IN_DEX] = {
		[AM_I] = OPC_DEX_I + 1,
	},
	[IN_DEY] = {
		[AM_I] = OPC_DEY_I + 1,
	},
	[IN_EOR] = {
		[AM_ABS] = OPC_EOR_ABS + 1,
		[AM_ABSX] = OPC_EOR_ABSX + 1,
		[AM_ABSY] = OPC_EOR_ABSY + 1,
		[AM_IM] = OPC_EOR_IM + 1,
		[AM_ZP] = OPC_EOR_ZP + 1,
		[AM_ZPI] = OPC_EOR_ZPI + 1,
		[AM_ZPII] = OPC_EOR_ZPII + 1,
		[AM_ZPIIY] = OPC_EOR_ZPIIY + 1,
		[AM_ZPX] = OPC_EOR_ZPX + 1,
	},
	[IN_INC] = {
		[AM_ABS] = OPC_INC_ABS + 1,
		[AM_ABSX] = OPC_INC_ABSX + 1,
		[AM_ACC] = OPC_INC_ACC + 1,
		[AM_ZP] = OPC_INC_ZP + 1,
		[AM_ZPX] = OPC_INC_ZPX + 1,
	},
	[IN_INX] = {
		[AM_I] = OPC_INX_I + 1,
	},
	[IN_INY] = {
		[AM_I] = OPC_INY_I + 1,
	},
	[IN_JMP] = {
		[AM_ABS] = OPC_JMP_ABS + 1,
		[AM_ABSI] = OPC_JMP_ABSI + 1,
		[AM_ABSII] = OPC_JMP_ABSII + 1,
	},
	[IN_JSR] = {
		[AM_ABS] = OPC_JSR_ABS + 1,
	},
	[IN_LDA] = {
		[AM_ABS] = OPC_LDA_ABS + 1,
		[AM_ABSX] = OPC_LDA_ABSX + 1,
		[AM_ABSY] = OPC_LDA_ABSY + 1,
		[AM_IM] = OPC_LDA_IM + 1,
		[AM_ZP] = OPC_LDA_ZP + 1,
		[AM_ZPI] = OPC_LDA_ZPI + 1,
		[AM_ZPII] = OPC_LDA_ZPII + 1,
		[AM_ZPIIY] = OPC_LDA_ZPIIY + 1,
		[AM_ZPX] = OPC_LDA_ZPX + 1,
	},
	[IN_LDX] = {
		[AM_ABS] = OPC_LDX_ABS + 1,
		[AM_ABSY] = OPC_LDX_ABSY + 1,
		[AM_IM] = OPC_LDX_IM + 1,
		[AM_ZP] = OPC_LDX_ZP + 1,
		[AM_ZPY] = OPC_LDX_ZPY
	},
	[IN_LDY] = {
		[AM_ABS] = OPC_LDY_ABS + 1,
		[AM_ABSX] = OPC_LDY_ABSX + 1,
		[AM_IM] = OPC_LDY_IM + 1,
		[AM_ZP] = OPC_LDY_ZP + 1,
		[AM_ZPX] = OPC_LDY_ZPX + 1,
	},
	[IN_LSR] = {
		[AM_ABS] = OPC_LSR_ABS + 1,
		[AM_ABSX] = OPC_LSR_ABSX + 1,
		[AM_ACC] = OPC_LSR_ACC + 1,
		[AM_ZP] = OPC_LSR_ZP + 1,
		[AM_ZPX] = OPC_LSR_ZPX + 1,
	},
	[IN_NOP] = {
		[AM_I] = OPC_NOP_I + 1,
	},
	[IN_ORA] = {
		[AM_ABS] = OPC_ORA_ABS + 1,
		[AM_ABSX] = OPC_ORA_ABSX + 1,
		[AM_ABSY] = OPC_ORA_ABSY + 1,
		[AM_IM] = OPC_ORA_IM + 1,
		[AM_ZP] = OPC_ORA_ZP + 1,
		[AM_ZPI] = OPC_ORA_ZPI + 1,
		[AM_ZPII] = OPC_ORA_ZPII + 1,
		[AM_ZPIIY] = OPC_ORA_ZPIIY + 1,
		[AM_ZPX] = OPC_ORA_ZPX + 1,
	},
	[IN_PHA] = {
		[AM_S] = OPC_PHA_S + 1,
	},
	[IN_PHP] = {
		[AM_S] = OPC_PHP_S + 1,
	},
	[IN_PHX] = {
		[AM_S] = OPC_PHX_S + 1,
	},
	[IN_PHY] = {
		[AM_S] = OPC_PHY_S + 1,
	},
	[IN_PLA] = {
		[AM_S] = OPC_PLA_S + 1,
	},
	[IN_PLP] = {
		[AM_S] = OPC_PLP_S + 1,
	},
	[IN_PLX] = {
		[AM_S] = OPC_PLX_S + 1,
	},
	[IN_PLY] = {
		[AM_S] = OPC_PLY_S + 1,
	},
	[IN_RMB0] = {
		[AM_ZP] = OPC_RMB0_ZP + 1,
	},
	[IN_RMB1] = {
		[AM_ZP] = OPC_RMB1_ZP + 1,
	},
	[IN_RMB2] = {
		[AM_ZP] = OPC_RMB2_ZP + 1,
	},
	[IN_RMB3] = {
		[AM_ZP] = OPC_RMB3_ZP + 1,
	},
	[IN_RMB4] = {
		[AM_ZP] = OPC_RMB4_ZP + 1,
	},
	[IN_RMB5] = {
		[AM_ZP] = OPC_RMB5_ZP + 1,
	},
	[IN_RMB6] = {
		[AM_ZP] = OPC_RMB6_ZP + 1,
	},
	[IN_RMB7] = {
		[AM_ZP] = OPC_RMB7_ZP + 1,
	},
	[IN_ROL] = {
		[AM_ABS] = OPC_ROL_ABS + 1,
		[AM_ABSX] = OPC_ROL_ABSX + 1,
		[AM_ACC] = OPC_ROL_ACC + 1,
		[AM_ZP] = OPC_ROL_ZP + 1,
		[AM_ZPX] = OPC_ROL_ZPX + 1,
	},
	[IN_ROR] = {
		[AM_ABS] = OPC_ROR_ABS + 1,
		[AM_ABSX] = OPC_ROR_ABSX + 1,
		[AM_ACC] = OPC_ROR_ACC + 1,
		[AM_ZP] = OPC_ROR_ZP + 1,
		[AM_ZPX] = OPC_ROR_ZPX + 1,
	},
	[IN_RTI] = {
		[AM_S] = OPC_RTI_S + 1,
	},
	[IN_RTS] = {
		[AM_S] = OPC_RTS_S + 1,
	},
	[IN_SBC] = {
		[AM_ABS] = OPC_SBC_ABS + 1,
		[AM_ABSX] = OPC_SBC_ABSX + 1,
		[AM_ABSY] = OPC_SBC_ABSY + 1,
		[AM_IM] = OPC_SBC_IM + 1,
		[AM_ZP] = OPC_SBC_ZP + 1,
		[AM_ZPI] = OPC_SBC_ZPI + 1,
		[AM_ZPII] = OPC_SBC_ZPII + 1,
		[AM_ZPIIY] = OPC_SBC_ZPIIY + 1,
		[AM_ZPX] = OPC_SBC_ZPX + 1,
	},
	[IN_SEC] = {
		[AM_I] = OPC_SEC_I + 1,
	},
	[IN_SED] = {
		[AM_I] = OPC_SED_I + 1,
	},
	[IN_SEI] = {
		[AM_I] = OPC_SEI_I + 1,
	},
	[IN_SMB0] = {
		[AM_ZP] = OPC_SMB0_ZP + 1,
	},
	[IN_SMB1] = {
		[AM_ZP] = OPC_SMB1_ZP + 1,
	},
	[IN_SMB2] = {
		[AM_ZP] = OPC_SMB2_ZP + 1,
	},
	[IN_SMB3] = {
		[AM_ZP] = OPC_SMB3_ZP + 1,
	},
	[IN_SMB4] = {
		[AM_ZP] = OPC_SMB4_ZP + 1,
	},
	[IN_SMB5] = {
		[AM_ZP] = OPC_SMB5_ZP + 1,
	},
	[IN_SMB6] = {
		[AM_ZP] = OPC_SMB6_ZP + 1,
	},
	[IN_SMB7] = {
		[AM_ZP] = OPC_SMB7_ZP + 1,
	},
	[IN_STA] = {
		[AM_ABS] = OPC_STA_ABS + 1,
		[AM_ABSX] = OPC_STA_ABSX + 1,
		[AM_ABSY] = OPC_STA_ABSY + 1,
		[AM_ZP] = OPC_STA_ZP + 1,
		[AM_ZPI] = OPC_STA_ZPI + 1,
		[AM_ZPII] = OPC_STA_ZPII + 1,
		[AM_ZPIIY] = OPC_STA_ZPIIY + 1,
		[AM_ZPX] = OPC_STA_ZPX + 1,
	},
	[IN_STP] = {
		[AM_I] = OPC_STP_I + 1,
	},
	[IN_STX] = {
		[AM_ABS] = OPC_STX_ABS + 1,
		[AM_ZP] = OPC_STX_ZP + 1,
		[AM_ZPY] = OPC_STX_ZPY
	},
	[IN_STY] = {
		[AM_ABS] = OPC_STY_ABS + 1,
		[AM_ZP] = OPC_STY_ZP + 1,
		[AM_ZPX] = OPC_STY_ZPX + 1,
	},
	[IN_STZ] = {
		[AM_ABS] = OPC_STZ_ABS + 1,
		[AM_ABSX] = OPC_STZ_ABSX + 1,
		[AM_ZP] = OPC_STZ_ZP + 1,
		[AM_ZPX] = OPC_STZ_ZPX + 1,
	},
	[IN_TAX] = {
		[AM_I] = OPC_TAX_I + 1,
	},
	[IN_TAY] = {
		[AM_I] = OPC_TAY_I + 1,
	},
	[IN_TRB] = {
		[AM_ABS] = OPC_TRB_ABS + 1,
		[AM_ZP] = OPC_TRB_ZP + 1,
	},
	[IN_TSB] = {
		[AM_ABS] = OPC_TSB_ABS + 1,
		[AM_ZP] = OPC_TSB_ZP + 1,
	},
	[IN_TSX] = {
		[AM_I] = OPC_TSX_I + 1,
	},
	[IN_TXA] = {
		[AM_I] = OPC_TXA_I + 1,
	},
	[IN_TXS] = {
		[AM_I] = OPC_TXS_I + 1,
	},
	[IN_TYA] = {
		[AM_I] = OPC_TYA_I + 1,
	},
	[IN_WAI] = {
		[AM_I] = OPC_WAI_I + 1,
	}
};

const char flagsList[][3] = {
	[AM_I] = {0},
	[AM_S] = {0},
	[AM_ACC] = {0},
	[AM_ABS] = {'V'},
	[AM_PCR] = {'Z'},
	[AM_ABSII] = {'V', 'X', 'N'},
	[AM_ABSX] = {'V', 'X'},
	[AM_ABSY] = {'V', 'Y'},
	[AM_ABSI] = {'V', 'N'},
	[AM_IM] = {'Z', 'I'},
	[AM_ZP] = {'Z'},
	[AM_ZPII] = {'Z', 'X', 'N'},
	[AM_ZPX] = {'Z', 'X'},
	[AM_ZPY] = {'Z', 'Y'},
	[AM_ZPI] = {'Z', 'N'},
	[AM_ZPIIY] = {'Z', 'N', 'Y'}
};
