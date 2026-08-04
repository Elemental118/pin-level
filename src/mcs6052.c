#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mcs6502.h"
#include "types.h"

#define FLAG_N 7
#define FLAG_V 6
#define FLAG_D 3
#define FLAG_I 2
#define FLAG_Z 1
#define FLAG_C 0

#define SET_FLAG(flag, val)	(c->priv->regs->p = (c->priv->regs->p & ~(1 << (flag))) | !!(val) << (flag))
#define GET_FLAG(flag)		((c->priv->regs->p & (1 << (flag))) >> (flag))

#define PC	((u16)c->priv->regs->pcl | (u16)c->priv->regs->pch << 8)

bool t;

static u8 data(struct mcs6502 *c)
{
	u8 data = 0;
	for (int i = 0; i < 8; i++) {
		data |= ((u8)*c->d[i] << i);
	}
	return data;
}

static void set_addr(struct mcs6502 *c, u16 addr)
{
	for (int i = 0; i < 16; i++) {
		*c->a[i] = (addr >> i) & 1;
	}
}

static void set_data(struct mcs6502 *c, u8 data)
{
	for (int i = 0; i < 8; i++) {
		*c->d[i] = (data >> i) & 1;
	}
}

static void bus_read(struct mcs6502 *c, u16 addr)
{
	*c->rw = true;
	set_addr(c, addr);
}

static void bus_write(struct mcs6502 *c, u16 addr, u8 data) {
	*c->rw = false;
	set_addr(c, addr);
	set_data(c, data);
}

enum op : u8 {
	OP_ILLEGAL,

	OP_ADC,
	OP_AND,
	OP_ASL,
	OP_BCC,
	OP_BCS,
	OP_BEQ,
	OP_BIT,
	OP_BMI,
	OP_BNE,
	OP_BPL,
	OP_BRK,
	OP_BVC,
	OP_BVS,
	OP_CLC,
	OP_CLD,
	OP_CLI,
	OP_CLV,
	OP_CMP,
	OP_CPX,
	OP_CPY,
	OP_DEC,
	OP_DEX,
	OP_DEY,
	OP_EOR,
	OP_INC,
	OP_INX,
	OP_INY,
	OP_JMP,
	OP_JSR,
	OP_LDA,
	OP_LDX,
	OP_LDY,
	OP_LSR,
	OP_NOP,
	OP_ORA,
	OP_PHA,
	OP_PHP,
	OP_PLA,
	OP_PLP,
	OP_ROL,
	OP_ROR,
	OP_RTI,
	OP_RTS,
	OP_SBC,
	OP_SEC,
	OP_SED,
	OP_SEI,
	OP_STA,
	OP_STX,
	OP_STY,
	OP_TAX,
	OP_TAY,
	OP_TSX,
	OP_TXA,
	OP_TXS,
	OP_TYA
};

enum group {
	GRP_ILLEGAL,
	GRP_R,
	GRP_W,
	GRP_RMW,
	GRP_NONE,

	GRP_COUNT
};

struct opdef {
	char *name;
	enum group g;
};

static const struct opdef ops[] = {
	{NULL,  GRP_ILLEGAL},
	{"ADC", GRP_R},
	{"AND", GRP_R},
	{"ASL", GRP_RMW},
	{"BCC", GRP_NONE},
	{"BCS", GRP_NONE},
	{"BEQ", GRP_NONE},
	{"BIT", GRP_R},
	{"BMI", GRP_NONE},
	{"BNE", GRP_NONE},
	{"BPL", GRP_NONE},
	{"BRK", GRP_NONE},
	{"BVC", GRP_NONE},
	{"BVS", GRP_NONE},
	{"CLC", GRP_NONE},
	{"CLD", GRP_NONE},
	{"CLI", GRP_NONE},
	{"CLV", GRP_NONE},
	{"CMP", GRP_R},
	{"CPX", GRP_R},
	{"CPY", GRP_R},
	{"DEC", GRP_RMW},
	{"DEX", GRP_NONE},
	{"DEY", GRP_NONE},
	{"EOR", GRP_R},
	{"INC", GRP_RMW},
	{"INX", GRP_NONE},
	{"INY", GRP_NONE},
	{"JMP", GRP_NONE},
	{"JSR", GRP_NONE},
	{"LDA", GRP_R},
	{"LDX", GRP_R},
	{"LDY", GRP_R},
	{"LSR", GRP_RMW},
	{"NOP", GRP_NONE},
	{"ORA", GRP_R},
	{"PHA", GRP_NONE},
	{"PHP", GRP_NONE},
	{"PLA", GRP_NONE},
	{"PLP", GRP_NONE},
	{"ROL", GRP_RMW},
	{"ROR", GRP_RMW},
	{"RTI", GRP_NONE},
	{"RTS", GRP_NONE},
	{"SBC", GRP_R},
	{"SEC", GRP_NONE},
	{"SED", GRP_NONE},
	{"SEI", GRP_NONE},
	{"STA", GRP_W},
	{"STX", GRP_W},
	{"STY", GRP_W},
	{"TAX", GRP_NONE},
	{"TAY", GRP_NONE},
	{"TSX", GRP_NONE},
	{"TXA", GRP_NONE},
	{"TXS", GRP_NONE},
	{"TYA", GRP_NONE},
};

enum mode : u8 {
	MODE_ILLEGAL,

	MODE_ABS,
	MODE_ABSX,
	MODE_ABSY,
	MODE_ACC,
	MODE_IMM,
	MODE_IMP,
	MODE_IND,
	MODE_INDX,
	MODE_INDY,
	MODE_REL,
	MODE_ZPG,
	MODE_ZPGX,
	MODE_ZPGY,

	MODE_COUNT
};

struct opinfo {
	enum op o;
	enum mode m;
};

static const struct opinfo optable[256] = {
	[0x00] = {OP_BRK, MODE_IMP},
	[0x01] = {OP_ORA, MODE_INDX},
	[0x05] = {OP_ORA, MODE_ZPG},
	[0x06] = {OP_ASL, MODE_ZPG},
	[0x08] = {OP_PHP, MODE_IMP},
	[0x09] = {OP_ORA, MODE_IMM},
	[0x0A] = {OP_ASL, MODE_ACC},
	[0x0D] = {OP_ORA, MODE_ABS},
	[0x0E] = {OP_ASL, MODE_ABS},

	[0x10] = {OP_BPL, MODE_REL},
	[0x11] = {OP_ORA, MODE_INDY},
	[0x15] = {OP_ORA, MODE_ZPGX},
	[0x16] = {OP_ASL, MODE_ZPGX},
	[0x18] = {OP_CLC, MODE_IMP},
	[0x19] = {OP_ORA, MODE_ABSY},
	[0x1D] = {OP_ORA, MODE_ABSX},
	[0x1E] = {OP_ASL, MODE_ABSX},

	[0x20] = {OP_JSR, MODE_ABS},
	[0x21] = {OP_AND, MODE_INDX},
	[0x24] = {OP_BIT, MODE_ZPG},
	[0x25] = {OP_AND, MODE_ZPG},
	[0x26] = {OP_ROL, MODE_ZPG},
	[0x28] = {OP_PLP, MODE_IMP},
	[0x29] = {OP_AND, MODE_IMM},
	[0x2A] = {OP_ROL, MODE_ACC},
	[0x2C] = {OP_BIT, MODE_ABS},
	[0x2D] = {OP_AND, MODE_ABS},
	[0x2E] = {OP_ROL, MODE_ABS},

	[0x30] = {OP_BMI, MODE_REL},
	[0x31] = {OP_AND, MODE_INDY},
	[0x35] = {OP_AND, MODE_ZPGX},
	[0x36] = {OP_ROL, MODE_ZPGX},
	[0x38] = {OP_SEC, MODE_IMP},
	[0x39] = {OP_AND, MODE_ABSY},
	[0x3D] = {OP_AND, MODE_ABSX},
	[0x3E] = {OP_ROL, MODE_ABSX},

	[0x40] = {OP_RTI, MODE_IMP},
	[0x41] = {OP_EOR, MODE_INDX},
	[0x45] = {OP_EOR, MODE_ZPG},
	[0x46] = {OP_LSR, MODE_ZPG},
	[0x48] = {OP_PHA, MODE_IMP},
	[0x49] = {OP_EOR, MODE_IMM},
	[0x4A] = {OP_LSR, MODE_ACC},
	[0x4C] = {OP_JMP, MODE_ABS},
	[0x4D] = {OP_EOR, MODE_ABS},
	[0x4E] = {OP_LSR, MODE_ABS},

	[0x50] = {OP_BVC, MODE_REL},
	[0x51] = {OP_EOR, MODE_INDY},
	[0x55] = {OP_EOR, MODE_ZPGX},
	[0x56] = {OP_LSR, MODE_ZPGX},
	[0x58] = {OP_CLI, MODE_IMP},
	[0x59] = {OP_EOR, MODE_ABSY},
	[0x5D] = {OP_EOR, MODE_ABSX},
	[0x5E] = {OP_LSR, MODE_ABSX},

	[0x60] = {OP_RTS, MODE_IMP},
	[0x61] = {OP_ADC, MODE_INDX},
	[0x65] = {OP_ADC, MODE_ZPG},
	[0x66] = {OP_ROR, MODE_ZPG},
	[0x68] = {OP_PLA, MODE_IMP},
	[0x69] = {OP_ADC, MODE_IMM},
	[0x6A] = {OP_ROR, MODE_ACC},
	[0x6C] = {OP_JMP, MODE_IND},
	[0x6D] = {OP_ADC, MODE_ABS},
	[0x6E] = {OP_ROR, MODE_ABS},

	[0x70] = {OP_BVS, MODE_REL},
	[0x71] = {OP_ADC, MODE_INDY},
	[0x75] = {OP_ADC, MODE_ZPGX},
	[0x76] = {OP_ROR, MODE_ZPGX},
	[0x78] = {OP_SEI, MODE_IMP},
	[0x79] = {OP_ADC, MODE_ABSY},
	[0x7D] = {OP_ADC, MODE_ABSX},
	[0x7E] = {OP_ROR, MODE_ABSX},

	[0x81] = {OP_STA, MODE_INDX},
	[0x84] = {OP_STY, MODE_ZPG},
	[0x85] = {OP_STA, MODE_ZPG},
	[0x86] = {OP_STX, MODE_ZPG},
	[0x88] = {OP_DEY, MODE_IMP},
	[0x8A] = {OP_TXA, MODE_IMP},
	[0x8C] = {OP_STY, MODE_ABS},
	[0x8D] = {OP_STA, MODE_ABS},
	[0x8E] = {OP_STX, MODE_ABS},

	[0x90] = {OP_BCC, MODE_REL},
	[0x91] = {OP_STA, MODE_INDY},
	[0x94] = {OP_STY, MODE_ZPGX},
	[0x95] = {OP_STA, MODE_ZPGX},
	[0x96] = {OP_STX, MODE_ZPGY},
	[0x98] = {OP_TYA, MODE_IMP},
	[0x99] = {OP_STA, MODE_ABSY},
	[0x9A] = {OP_TXS, MODE_IMP},
	[0x9D] = {OP_STA, MODE_ABSX},

	[0xA0] = {OP_LDY, MODE_IMM},
	[0xA1] = {OP_LDA, MODE_INDX},
	[0xA2] = {OP_LDX, MODE_IMM},
	[0xA4] = {OP_LDY, MODE_ZPG},
	[0xA5] = {OP_LDA, MODE_ZPG},
	[0xA6] = {OP_LDX, MODE_ZPG},
	[0xA8] = {OP_TAY, MODE_IMP},
	[0xA9] = {OP_LDA, MODE_IMM},
	[0xAA] = {OP_TAX, MODE_IMP},
	[0xAC] = {OP_LDY, MODE_ABS},
	[0xAD] = {OP_LDA, MODE_ABS},
	[0xAE] = {OP_LDX, MODE_ABS},

	[0xB0] = {OP_BCS, MODE_REL},
	[0xB1] = {OP_LDA, MODE_INDY},
	[0xB4] = {OP_LDY, MODE_ZPGX},
	[0xB5] = {OP_LDA, MODE_ZPGX},
	[0xB6] = {OP_LDX, MODE_ZPGY},
	[0xB8] = {OP_CLV, MODE_IMP},
	[0xB9] = {OP_LDA, MODE_ABSY},
	[0xBA] = {OP_TSX, MODE_IMP},
	[0xBC] = {OP_LDY, MODE_ABSX},
	[0xBD] = {OP_LDA, MODE_ABSX},
	[0xBE] = {OP_LDX, MODE_ABSY},

	[0xC0] = {OP_CPY, MODE_IMM},
	[0xC1] = {OP_CMP, MODE_INDX},
	[0xC4] = {OP_CPY, MODE_ZPG},
	[0xC5] = {OP_CMP, MODE_ZPG},
	[0xC6] = {OP_DEC, MODE_ZPG},
	[0xC8] = {OP_INY, MODE_IMP},
	[0xC9] = {OP_CMP, MODE_IMM},
	[0xCA] = {OP_DEX, MODE_IMP},
	[0xCC] = {OP_CPY, MODE_ABS},
	[0xCD] = {OP_CMP, MODE_ABS},
	[0xCE] = {OP_DEC, MODE_ABS},

	[0xD0] = {OP_BNE, MODE_REL},
	[0xD1] = {OP_CMP, MODE_INDY},
	[0xD5] = {OP_CMP, MODE_ZPGX},
	[0xD6] = {OP_DEC, MODE_ZPGX},
	[0xD8] = {OP_CLD, MODE_IMP},
	[0xD9] = {OP_CMP, MODE_ABSY},
	[0xDD] = {OP_CMP, MODE_ABSX},
	[0xDE] = {OP_DEC, MODE_ABSX},

	[0xE0] = {OP_CPX, MODE_IMM},
	[0xE1] = {OP_SBC, MODE_INDX},
	[0xE4] = {OP_CPX, MODE_ZPG},
	[0xE5] = {OP_SBC, MODE_ZPG},
	[0xE6] = {OP_INC, MODE_ZPG},
	[0xE8] = {OP_INX, MODE_IMP},
	[0xE9] = {OP_SBC, MODE_IMM},
	[0xEA] = {OP_NOP, MODE_IMP},
	[0xEC] = {OP_CPX, MODE_ABS},
	[0xED] = {OP_SBC, MODE_ABS},
	[0xEE] = {OP_INC, MODE_ABS},

	[0xF0] = {OP_BEQ, MODE_REL},
	[0xF1] = {OP_SBC, MODE_INDY},
	[0xF5] = {OP_SBC, MODE_ZPGX},
	[0xF6] = {OP_INC, MODE_ZPGX},
	[0xF8] = {OP_SED, MODE_IMP},
	[0xF9] = {OP_SBC, MODE_ABSY},
	[0xFD] = {OP_SBC, MODE_ABSX},
	[0xFE] = {OP_INC, MODE_ABSX},
};

struct reg_file {
	u8 a;
	u8 x;
	u8 y;
	u8 p;
	u8 s;
	u8 pch;
	u8 pcl;
};

struct mcs6502_priv {
	int  cycle;
	u8   opcode;
	u16  operand;
	u16  instruction_begin_pc;

	bool scratch1;
	u8   scratch8;
	u16  scratch16;

	bool irq_take;
	bool irq_now;
	bool irq_running;

	bool nmi_take;
	bool nmi_now;
	bool nmi_pending;
	bool nmi_prev;
	bool nmi_running;

	bool reset_pending;
	bool reset_running;

	bool phi0_prev;

	enum op o;
	enum mode m;
	enum group g;

	struct reg_file *regs;
};

static void print_instruction(struct mcs6502 *c)
{
	if (c->priv->irq_running) {
		fprintf(stderr, "---- IRQ ----\n");
		return;
	} else if (c->priv->nmi_running) {
		fprintf(stderr, "---- NMI ----\n");
		return;
	} else if (c->priv->reset_running) {
		fprintf(stderr, "--- RESET ---\n");
		return;
	}

	fprintf(stderr, "<0x%04X>      %s ", c->priv->instruction_begin_pc, ops[c->priv->o].name);
	switch (c->priv->m) {
	case MODE_ABS:
		fprintf(stderr, "$%04X  ", c->priv->operand);
		break;
	
	case MODE_ABSX:
		fprintf(stderr, "$%04X,X", c->priv->operand);
		break;
	
	case MODE_ABSY:
		fprintf(stderr, "$%04X,Y", c->priv->operand);
		break;
	
	case MODE_ACC:
		fprintf(stderr, "A      ");
		break;
	
	case MODE_IMM:
		fprintf(stderr, "#$%02X   ", (u8)c->priv->operand);
		break;
	
	case MODE_IMP:
		fprintf(stderr, "       ");
		break;
	
	case MODE_IND:
		fprintf(stderr, "($%04X)", c->priv->operand);
		break;
	
	case MODE_INDX:
		fprintf(stderr, "($%02X,X)", (u8)c->priv->operand);
		break;
	
	case MODE_INDY:
		fprintf(stderr, "($%02X),Y", (u8)c->priv->operand);
		break;
	
	case MODE_REL:
		fprintf(stderr, "$%04X  ", (s8)c->priv->operand + c->priv->instruction_begin_pc + 2);
		break;
	
	case MODE_ZPG:
		fprintf(stderr, "$%02X    ", (u8)c->priv->operand);
		break;
	
	case MODE_ZPGX:
		fprintf(stderr, "$%02X,X  ", (u8)c->priv->operand);
		break;
	
	case MODE_ZPGY:
		fprintf(stderr, "$%02X,Y  ", (u8)c->priv->operand);
		break;
	
	case MODE_ILLEGAL:
	default:
		fprintf(stderr, "attempted illegal operation\n");
		
	}

	fprintf(stderr, "      A:0x%02X X:0x%02X Y:0x%02X P:0x%02X SP:0x%02X\n", c->priv->regs->a, c->priv->regs->x, c->priv->regs->y, c->priv->regs->p, c->priv->regs->s);
}

static void pc_inc(struct mcs6502 *c)
{
	if (++c->priv->regs->pcl == 0) {
		c->priv->regs->pch++;
	}
}

static void apply_read_op(struct mcs6502 *c, u8 val)
{
	switch (c->priv->o) {
	case OP_LDA:
		c->priv->regs->a = val;
		SET_FLAG(FLAG_Z, !val);
		SET_FLAG(FLAG_N, val & 0b10000000);
		break;
	
	case OP_LDX:
		c->priv->regs->x = val;
		SET_FLAG(FLAG_Z, !val);
		SET_FLAG(FLAG_N, val & 0b10000000);
		break;
	
	case OP_LDY:
		c->priv->regs->y = val;
		SET_FLAG(FLAG_Z, !val);
		SET_FLAG(FLAG_N, val & 0b10000000);
		break;
	
	case OP_AND:
		c->priv->regs->a &= val;
		SET_FLAG(FLAG_Z, !c->priv->regs->a);
		SET_FLAG(FLAG_N, c->priv->regs->a & 0b10000000);
		break;
	
	case OP_ORA:
		c->priv->regs->a |= val;
		SET_FLAG(FLAG_Z, !c->priv->regs->a);
		SET_FLAG(FLAG_N, c->priv->regs->a & 0b10000000);
		break;

	case OP_EOR:
		c->priv->regs->a ^= val;
		SET_FLAG(FLAG_Z, !c->priv->regs->a);
		SET_FLAG(FLAG_N, c->priv->regs->a & 0b10000000);
		break;
	
	case OP_CMP:
		SET_FLAG(FLAG_C, c->priv->regs->a >= val);
		SET_FLAG(FLAG_Z, c->priv->regs->a == val);
		SET_FLAG(FLAG_N, (c->priv->regs->a - val) & 0b10000000);
		break;

	case OP_CPX:
		SET_FLAG(FLAG_C, c->priv->regs->x >= val);
		SET_FLAG(FLAG_Z, c->priv->regs->x == val);
		SET_FLAG(FLAG_N, (c->priv->regs->x - val) & 0b10000000);
		break;

	case OP_CPY:
		SET_FLAG(FLAG_C, c->priv->regs->y >= val);
		SET_FLAG(FLAG_Z, c->priv->regs->y == val);
		SET_FLAG(FLAG_N, (c->priv->regs->y - val) & 0b10000000);
		break;

	case OP_BIT:
		SET_FLAG(FLAG_Z, !(c->priv->regs->a & val));
		SET_FLAG(FLAG_N, val & 0b10000000);
		SET_FLAG(FLAG_V, val & 0b01000000);
		break;
	
	case OP_ADC:
		bool c_in_adc = GET_FLAG(FLAG_C);
		u16 sum = c->priv->regs->a + val + c_in_adc;
		SET_FLAG(FLAG_Z, !(u8)(sum));

		if (GET_FLAG(FLAG_D)) {
			u8 low_nibble_adc = (c->priv->regs->a & 0x0F) + (val & 0x0F) + c_in_adc;
			u8 high_nibble_adc = (c->priv->regs->a >> 4) + (val >> 4);
			if (low_nibble_adc >= 0x0A) {
				low_nibble_adc += 6;
				high_nibble_adc++;
			}
			SET_FLAG(FLAG_C, high_nibble_adc >= 0x0A);
			SET_FLAG(FLAG_N, (((high_nibble_adc << 4) | (low_nibble_adc & 0x0F)) & 0b10000000));
			SET_FLAG(FLAG_V, (~(c->priv->regs->a ^ val) & (c->priv->regs->a ^ ((high_nibble_adc << 4) | (low_nibble_adc & 0x0F)))) & 0b10000000);

			if (high_nibble_adc >= 0x0A) {
				high_nibble_adc += 6;
			}
			c->priv->regs->a = (high_nibble_adc << 4) | (low_nibble_adc & 0x0F);
		} else {
			SET_FLAG(FLAG_C, sum > 0xFF);
			SET_FLAG(FLAG_N, (sum & 0b10000000));
			SET_FLAG(FLAG_V, (~(c->priv->regs->a ^ val) & (c->priv->regs->a ^ sum)) & 0b10000000);
			c->priv->regs->a = sum;
		}
		break;
	
	case OP_SBC:
		bool c_in_sbc = GET_FLAG(FLAG_C);
		u16 diff = c->priv->regs->a + (u8)~val + c_in_sbc;
		SET_FLAG(FLAG_C, diff > 0xFF);
		SET_FLAG(FLAG_Z, !(u8)(diff));
		SET_FLAG(FLAG_N, (diff & 0b10000000));
		SET_FLAG(FLAG_V, ((c->priv->regs->a ^ val) & (c->priv->regs->a ^ diff)) & 0b10000000);
		if (GET_FLAG(FLAG_D)) {
			u8 low_nibble_sbc = (c->priv->regs->a & 0x0F) - (val & 0x0F) + c_in_sbc - 1;
			u8 high_nibble_sbc = (c->priv->regs->a >> 4) - (val >> 4);
			if (low_nibble_sbc >= 0xF0) {
				low_nibble_sbc -= 6;
				high_nibble_sbc--;
			}
			if (high_nibble_sbc >= 0xF0) {
				high_nibble_sbc -= 6;
			}
			c->priv->regs->a = (high_nibble_sbc << 4) | (low_nibble_sbc & 0x0F);
		} else {
			c->priv->regs->a = diff;
		}
		break;
	
	case OP_ILLEGAL:
	default:
		fprintf(stderr, "attempted illegal operation\n");
	}
}

static u8 apply_write_op(struct mcs6502 *c)
{
	switch (c->priv->o) {
	case OP_STA:
		return c->priv->regs->a;
	case OP_STX:
		return c->priv->regs->x;
	case OP_STY:
		return c->priv->regs->y;
	case OP_ILLEGAL:
	default:
		fprintf(stderr, "attempted illegal operation\n");
		return 0;
	}
}

static u8 apply_rmw_op(struct mcs6502 *c, u8 val)
{
	switch (c->priv->o) {
	case OP_ASL:
		SET_FLAG(FLAG_C, val & 0b10000000);
		SET_FLAG(FLAG_Z, !(u8)(val << 1));
		SET_FLAG(FLAG_N, (val << 1) & 0b10000000);
		return val << 1;
	
	case OP_LSR:
		SET_FLAG(FLAG_C, val & 0b00000001);
		SET_FLAG(FLAG_Z, !(val >> 1));
		SET_FLAG(FLAG_N, (val >> 1) & 0b10000000);
		return val >> 1;
	
	case OP_ROL:
		u8 old_carry_rol = GET_FLAG(FLAG_C);
		SET_FLAG(FLAG_C, val & 0b10000000);
		SET_FLAG(FLAG_Z, !((u8)(val << 1) | old_carry_rol));
		SET_FLAG(FLAG_N, (val << 1) & 0b10000000);
		return (val << 1) | old_carry_rol;

	case OP_ROR:
		u8 old_carry_ror = GET_FLAG(FLAG_C);
		SET_FLAG(FLAG_C, val & 0b00000001);
		SET_FLAG(FLAG_Z, !((val >> 1) | (old_carry_ror << 7)));
		SET_FLAG(FLAG_N, ((val >> 1) | (old_carry_ror << 7)) & 0b10000000);
		return (val >> 1) | (old_carry_ror << 7);
	
	case OP_INC:
		SET_FLAG(FLAG_Z, !(u8)(val + 1));
		SET_FLAG(FLAG_N, (val + 1) & 0b10000000);
		return val + 1;
	
	case OP_DEC:
		SET_FLAG(FLAG_Z, !(u8)(val - 1));
		SET_FLAG(FLAG_N, (val - 1) & 0b10000000);
		return val - 1;
	
	case OP_ILLEGAL:
	default:
		fprintf(stderr, "attempted illegal operation\n");
		return 0;
	}
}

static void apply_imp_op(struct mcs6502 *c)
{
	switch (c->priv->o) {
	case OP_TAX:
		c->priv->regs->x = c->priv->regs->a;
		SET_FLAG(FLAG_Z, !c->priv->regs->x);
		SET_FLAG(FLAG_N, c->priv->regs->x & 0b10000000);
		break;
	
	case OP_TAY:
		c->priv->regs->y = c->priv->regs->a;
		SET_FLAG(FLAG_Z, !c->priv->regs->y);
		SET_FLAG(FLAG_N, c->priv->regs->y & 0b10000000);
		break;
	
	case OP_TXA:
		c->priv->regs->a = c->priv->regs->x;
		SET_FLAG(FLAG_Z, !c->priv->regs->a);
		SET_FLAG(FLAG_N, c->priv->regs->a & 0b10000000);
		break;
	
	case OP_TYA:
		c->priv->regs->a = c->priv->regs->y;
		SET_FLAG(FLAG_Z, !c->priv->regs->a);
		SET_FLAG(FLAG_N, c->priv->regs->a & 0b10000000);
		break;
	
	case OP_TSX:
		c->priv->regs->x = c->priv->regs->s;
		SET_FLAG(FLAG_Z, !c->priv->regs->x);
		SET_FLAG(FLAG_N, c->priv->regs->x & 0b10000000);
		break;
	
	case OP_TXS:
		c->priv->regs->s = c->priv->regs->x;
		break;

	case OP_INX:
		c->priv->regs->x++;
		SET_FLAG(FLAG_Z, !c->priv->regs->x);
		SET_FLAG(FLAG_N, c->priv->regs->x & 0b10000000);
		break;
	
	case OP_DEX:
		c->priv->regs->x--;
		SET_FLAG(FLAG_Z, !c->priv->regs->x);
		SET_FLAG(FLAG_N, c->priv->regs->x & 0b10000000);
		break;
	
	case OP_INY:
		c->priv->regs->y++;
		SET_FLAG(FLAG_Z, !c->priv->regs->y);
		SET_FLAG(FLAG_N, c->priv->regs->y & 0b10000000);
		break;
	
	case OP_DEY:
		c->priv->regs->y--;
		SET_FLAG(FLAG_Z, !c->priv->regs->y);
		SET_FLAG(FLAG_N, c->priv->regs->y & 0b10000000);
		break;
	
	case OP_CLC:
		SET_FLAG(FLAG_C, 0);
		break;
	
	case OP_SEC:
		SET_FLAG(FLAG_C, 1);
		break;
	
	case OP_CLI:
		SET_FLAG(FLAG_I, 0);
		break;
	
	case OP_SEI:
		SET_FLAG(FLAG_I, 1);
		break;
	
	case OP_CLD:
		SET_FLAG(FLAG_D, 0);
		break;
	
	case OP_SED:
		SET_FLAG(FLAG_D, 1);
		break;
	
	case OP_CLV:
		SET_FLAG(FLAG_V, 0);
		break;
	
	case OP_NOP:
		break;
	
	case OP_ILLEGAL:
	default:
		fprintf(stderr, "attempted illegal operation\n");
		
	}
}

static bool branch_taken(struct mcs6502 *c) {
	switch (c->priv->o) {
	case OP_BCC:
		return !GET_FLAG(FLAG_C);
	case OP_BCS:
		return GET_FLAG(FLAG_C);
	case OP_BNE:
		return !GET_FLAG(FLAG_Z);
	case OP_BEQ:
		return GET_FLAG(FLAG_Z);
	case OP_BPL:
		return !GET_FLAG(FLAG_N);
	case OP_BMI:
		return GET_FLAG(FLAG_N);
	case OP_BVC:
		return !GET_FLAG(FLAG_V);
	case OP_BVS:
		return GET_FLAG(FLAG_V);
	case OP_ILLEGAL:
	default:
		fprintf(stderr, "attempted illegal operation\n");
		return 0;
	}
}

struct mcs6502 *mcs6502_create(bool trace)
{
	struct mcs6502 *c = calloc(1, sizeof(*c));
	c->priv = calloc(1, sizeof(*c->priv));
	c->priv->regs = calloc(1, sizeof(*c->priv->regs));
	c->priv->cycle = 1;
	c->priv->regs->p = 0b00100000;
	c->priv->phi0_prev = true;
	c->priv->nmi_prev = true;
	t = trace;
	return c;
}

void mcs6502_free(struct mcs6502 *c)
{
	free(c->priv);
	free(c);
}

void mcs6502_tick(struct mcs6502 *c)
{
	if (c->priv->reset_pending) {
		c->priv->nmi_pending = false;
		c->priv->nmi_now = false;
		c->priv->nmi_take = false;

		c->priv->irq_running = false;
		c->priv->nmi_running = false;

		c->priv->reset_pending = false;
		c->priv->reset_running = true;
		c->priv->cycle = 1;
	}
	if (*c->res) {
		c->priv->reset_pending = true;
		return;
	}
	bool edge = !*c->phi0 && c->priv->phi0_prev;
	c->priv->phi0_prev = *c->phi0;
	if (!edge) {
		return;
	}
	if (c->priv->cycle == 1) {
		if (c->priv->irq_take || c->priv->nmi_take || c->priv->reset_running) {
			c->priv->o = OP_BRK;
			c->priv->m = MODE_IMP;
			c->priv->g = ops[c->priv->o].g;
			c->priv->irq_running = c->priv->irq_take && !c->priv->nmi_take && !c->priv->reset_running;
			c->priv->nmi_running = c->priv->nmi_take && !c->priv->reset_running;
			bus_read(c, PC);
			goto tick_end;
		}

		c->priv->instruction_begin_pc = PC;
		c->priv->opcode = data(c);
		c->priv->o = optable[c->priv->opcode].o;
		c->priv->m = optable[c->priv->opcode].m;
		c->priv->g = ops[c->priv->o].g;
		pc_inc(c);
		bus_read(c, PC);
		goto tick_end;
	}

	switch (c->priv->m) {
	case MODE_ABS:
		if (c->priv->o == OP_JMP) {
			switch (c->priv->cycle) {
			case 2:
				c->priv->operand = data(c);
				pc_inc(c);
				bus_read(c, PC);
				break;
			
			case 3:
				c->priv->scratch8 = data(c);
				c->priv->regs->pcl = (u8)c->priv->operand;
				c->priv->regs->pch = c->priv->scratch8;
				c->priv->operand |= ((u16)c->priv->scratch8 << 8);
				c->priv->cycle = 0;
				break;
				
			default:
				fprintf(stderr, "illegal cycle count\n");
				
			}
			break;
		} else if (c->priv->o == OP_JSR) {
			switch (c->priv->cycle) {
			case 2:
				c->priv->operand = data(c);
				pc_inc(c);
				bus_read(c, 0x100 | c->priv->regs->s);
				break;
			
			case 3:
				bus_write(c, 0x100 | c->priv->regs->s, c->priv->regs->pch);
				break;
			
			case 4:
				c->priv->regs->s--;
				bus_write(c, 0x100 | c->priv->regs->s, c->priv->regs->pcl);
				break;
			
			case 5:
				c->priv->regs->s--;
				bus_read(c, PC);
				break;
			
			case 6:
				c->priv->scratch8 = data(c);
				c->priv->regs->pcl = c->priv->operand;
				c->priv->regs->pch = c->priv->scratch8;
				c->priv->operand |= ((u16)c->priv->scratch8 << 8);
				c->priv->cycle = 0;
				break;
				
			default:
				fprintf(stderr, "illegal cycle count\n");
				
			}
			break;
		}
		switch (c->priv->cycle) {
		case 2:
			c->priv->operand = data(c);
			pc_inc(c);
			bus_read(c, PC);
			break;

		case 3:
			c->priv->operand |= ((u16)data(c) << 8);
			pc_inc(c);
			switch (c->priv->g) {
			case GRP_R:
				bus_read(c, c->priv->operand);
				break;
			
			case GRP_W:
				bus_write(c, c->priv->operand, apply_write_op(c));
				break;
			
			case GRP_RMW:
				bus_read(c, c->priv->operand);
				break;
			
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;

		case 4:
			switch (c->priv->g) {
			case GRP_R:
				apply_read_op(c, data(c));
				c->priv->cycle = 0;
				break;
			
			case GRP_W:
				c->priv->cycle = 0;
				break;
			
			case GRP_RMW:
				c->priv->scratch8 = data(c);
				bus_write(c, c->priv->operand, c->priv->scratch8);
				break;
			
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;
		
		case 5:
			c->priv->scratch8 = apply_rmw_op(c, c->priv->scratch8);
			bus_write(c, c->priv->operand, c->priv->scratch8);
			break;

		case 6:
			c->priv->cycle = 0;
			break;
		
		default:
			fprintf(stderr, "illegal cycle count\n");
			
		}
		break;
	
	case MODE_ABSX:
		switch (c->priv->cycle) {
		case 2:
			c->priv->operand = data(c);
			c->priv->scratch16 = c->priv->operand;
			pc_inc(c);
			bus_read(c, PC);
			break;

		case 3:
			c->priv->scratch8 = data(c);
			c->priv->operand = c->priv->scratch16 | ((u16)c->priv->scratch8 << 8);
			c->priv->scratch16 += c->priv->regs->x;
			c->priv->scratch1 = (c->priv->scratch16 <= 0xFF);
			pc_inc(c);
			if ((c->priv->g == GRP_R) && (c->priv->scratch1)) {
				c->priv->scratch16 = c->priv->scratch16 | ((u16)c->priv->scratch8 << 8);
				bus_read(c, c->priv->scratch16);
			} else if (c->priv->scratch1) {
				c->priv->scratch16 = (c->priv->scratch16) | ((u16)c->priv->scratch8 << 8);
				bus_read(c, c->priv->scratch16);
			} else {
				c->priv->scratch16 = (c->priv->scratch16 & 0xFF) | ((u16)c->priv->scratch8 << 8);
				bus_read(c, c->priv->scratch16);
			}
			break;
		
		case 4:
			if ((c->priv->g == GRP_R) && (c->priv->scratch1)) {
				apply_read_op(c, data(c));
				c->priv->cycle = 0;
				break;
			} else if (!c->priv->scratch1) {
				c->priv->scratch16 += 0x100;
			}

			switch (c->priv->g) {
			case GRP_R:
				bus_read(c, c->priv->scratch16);
				break;
			
			case GRP_W:
				bus_write(c, c->priv->scratch16, apply_write_op(c));
				break;
			
			case GRP_RMW:
				bus_read(c, c->priv->scratch16);
				break;
			
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;

		case 5:
			switch (c->priv->g) {
			case GRP_R:
				apply_read_op(c, data(c));
				c->priv->cycle = 0;
				break;
			
			case GRP_W:
				c->priv->cycle = 0;
				break;
			
			case GRP_RMW:
				c->priv->scratch8 = data(c);
				bus_write(c, c->priv->scratch16, c->priv->scratch8);
				break;
			
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;
		
		case 6:
			c->priv->scratch8 = apply_rmw_op(c, c->priv->scratch8);
			bus_write(c, c->priv->scratch16, c->priv->scratch8);
			break;

		case 7:
			c->priv->cycle = 0;
			break;
		
		default:
			fprintf(stderr, "illegal cycle count\n");
			
		}
		break;
	
	case MODE_ABSY:
		switch (c->priv->cycle) {
		case 2:
			c->priv->operand = data(c);
			c->priv->scratch16 = c->priv->operand;
			pc_inc(c);
			bus_read(c, PC);
			break;

		case 3:
			c->priv->scratch8 = data(c);
			c->priv->operand = c->priv->scratch16 | ((u16)c->priv->scratch8 << 8);
			c->priv->scratch16 += c->priv->regs->y;
			c->priv->scratch1 = (c->priv->scratch16 <= 0xFF);
			pc_inc(c);
			if ((c->priv->g == GRP_R) && (c->priv->scratch1)) {
				c->priv->scratch16 = c->priv->scratch16 | ((u16)c->priv->scratch8 << 8);
				bus_read(c, c->priv->scratch16);
			} else if (c->priv->scratch1) {
				c->priv->scratch16 = (c->priv->scratch16) | ((u16)c->priv->scratch8 << 8);
				bus_read(c, c->priv->scratch16);
			} else {
				c->priv->scratch16 = (c->priv->scratch16 & 0xFF) | ((u16)c->priv->scratch8 << 8);
				bus_read(c, c->priv->scratch16);
			}
			break;
		
		case 4:
			if ((c->priv->g == GRP_R) && (c->priv->scratch1)) {
				apply_read_op(c, data(c));
				c->priv->cycle = 0;
				break;
			} else if (!c->priv->scratch1) {
				c->priv->scratch16 += 0x100;
			}

			switch (c->priv->g) {
			case GRP_R:
				bus_read(c, c->priv->scratch16);
				break;
			
			case GRP_W:
				bus_write(c, c->priv->scratch16, apply_write_op(c));
				break;
			
			case GRP_RMW:
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;
		
		case 5:
			switch (c->priv->g) {
			case GRP_R:
				apply_read_op(c, data(c));
				c->priv->cycle = 0;
				break;
			
			case GRP_W:
				c->priv->cycle = 0;
				break;
			
			case GRP_RMW:			
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;

		default:
			fprintf(stderr, "illegal cycle count\n");
			
		}
		break;
	
	case MODE_ACC:
		switch (c->priv->cycle) {
		case 2:
			c->priv->regs->a = apply_rmw_op(c, c->priv->regs->a);
			c->priv->cycle = 0;
			break;

		default:
			fprintf(stderr, "illegal cycle count\n");
			
		}
		break;
	
	case MODE_IMM:
		switch (c->priv->cycle) {
		case 2:
			c->priv->operand = data(c);
			apply_read_op(c, c->priv->operand);
			pc_inc(c);
			c->priv->cycle = 0;
			break;

		default:
			fprintf(stderr, "illegal cycle count\n");
			
		}
		break;
	
	case MODE_IMP:
		switch (c->priv->o) {
		case OP_BRK:
			switch (c->priv->cycle) {
			case 2:
				if (!c->priv->irq_running && !c->priv->nmi_running && !c->priv->reset_running) {
					pc_inc(c);
				}

				if (c->priv->reset_running) {
					bus_read(c, 0x100 | c->priv->regs->s--);
				} else {
					bus_write(c, 0x100 | c->priv->regs->s--, c->priv->regs->pch);
				}
				break;
			
			case 3:
				if (c->priv->reset_running) {
					bus_read(c, 0x100 | c->priv->regs->s--);
				} else {
					bus_write(c, 0x100 | c->priv->regs->s--, c->priv->regs->pcl);
				}
				break;
			
			case 4:
				if (!c->priv->irq_running && !c->priv->nmi_running && !c->priv->reset_running) {
					bus_write(c, 0x100 | c->priv->regs->s--, c->priv->regs->p | 0b00110000);
				} else if (c->priv->reset_running) {
					bus_read(c, 0x100 | c->priv->regs->s--);
				} else {
					bus_write(c, 0x100 | c->priv->regs->s--, c->priv->regs->p | 0b00100000);
				}
				break;
			
			case 5:
				SET_FLAG(FLAG_I, 1);
				if (c->priv->nmi_running) {
					bus_read(c, 0xFFFA);
				} else if (c->priv->reset_running) {
					bus_read(c, 0xFFFC);
				} else {
					bus_read(c, 0xFFFE);
				}
				break;
			
			case 6:
				c->priv->regs->pcl = data(c);
				if (c->priv->nmi_running) {
					bus_read(c, 0xFFFB);
				} else if (c->priv->reset_running) {
					bus_read(c, 0xFFFD);
				} else {
					bus_read(c, 0xFFFF);
				}
				break;
			
			case 7:
				c->priv->regs->pch = data(c);
				c->priv->cycle = 0;
				break;
				
			default:
				fprintf(stderr, "illegal cycle count\n");
				
			}
			break;
		
		case OP_PHA:
			switch (c->priv->cycle) {
			case 2:
				bus_write(c, 0x100 | c->priv->regs->s--, c->priv->regs->a);
				break;
			
			case 3:
				c->priv->cycle = 0;
				break;
				
			default:
				fprintf(stderr, "illegal cycle count\n");
				
			}
			break;
		
		case OP_PHP:
			switch (c->priv->cycle) {
			case 2:
				bus_write(c, 0x100 | c->priv->regs->s--, c->priv->regs->p | 0b00110000);
				break;
			
			case 3:
				c->priv->cycle = 0;
				break;
				
			default:
				fprintf(stderr, "illegal cycle count\n");
				
			}
			break;
		
		case OP_PLA:
			switch (c->priv->cycle) {
			case 2:
				bus_read(c, 0x100 | c->priv->regs->s++);
				break;
			
			case 3:
				bus_read(c, 0x100 | c->priv->regs->s);
				break;
			
			case 4:
				c->priv->regs->a = data(c);
				SET_FLAG(FLAG_Z, !c->priv->regs->a);
				SET_FLAG(FLAG_N, c->priv->regs->a & 0b10000000);
				c->priv->cycle = 0;
				break;
				
			default:
				fprintf(stderr, "illegal cycle count\n");
				
			}
			break;
		
		case OP_PLP:
			switch (c->priv->cycle) {
			case 2:
				bus_read(c, 0x100 | c->priv->regs->s++);
				break;
			
			case 3:
				bus_read(c, 0x100 | c->priv->regs->s);
				break;

			case 4:
				c->priv->cycle = 0;
				c->priv->regs->p = (data(c) & 0b11101111) | 0b00100000;
				break;
				
			default:
				fprintf(stderr, "illegal cycle count\n");
				
			}
			break;
		
		case OP_RTS:
			switch (c->priv->cycle) {
			case 2:
				bus_read(c, 0x100 | c->priv->regs->s++);
				break;
			
			case 3:
				bus_read(c, 0x100 | c->priv->regs->s++);
				break;
			
			case 4:
				c->priv->regs->pcl = data(c);
				bus_read(c, 0x100 | c->priv->regs->s);
				break;
			
			case 5:
				c->priv->regs->pch = data(c);
				bus_read(c, PC);
				break;
			
			case 6:
				pc_inc(c);
				c->priv->cycle = 0;
				break;
				
			default:
				fprintf(stderr, "illegal cycle count\n");
				
			}
			break;
		
		case OP_RTI:
			switch (c->priv->cycle) {
			case 2:
				bus_read(c, 0x100 | c->priv->regs->s++);
				break;
			
			case 3:
				bus_read(c, 0x100 | c->priv->regs->s++);
				break;
			
			case 4:
				c->priv->regs->p = (data(c) & 0b11101111) | 0b00100000;
				bus_read(c, 0x100 | c->priv->regs->s++);
				break;
			
			case 5:
				c->priv->regs->pcl = data(c);
				bus_read(c, 0x100 | c->priv->regs->s);
				break;
			
			case 6:
				c->priv->regs->pch = data(c);
				c->priv->cycle = 0;
				break;
				
			default:
				fprintf(stderr, "illegal cycle count\n");
				
			}
			break;
		
		case OP_ILLEGAL:
			fprintf(stderr, "attempted illegal operation\n");
			

		default:
			switch (c->priv->cycle) {
			case 2:
				apply_imp_op(c);
				c->priv->cycle = 0;
				break;
			
			default:
				fprintf(stderr, "illegal cycle count\n");
				
			}
			break;
		}
		break;
	
	case MODE_IND:
		switch (c->priv->cycle) {
		case 2:
			c->priv->operand = data(c);
			pc_inc(c);
			bus_read(c, PC);
			break;
		
		case 3:
			c->priv->operand |= ((u16)data(c) << 8);
			pc_inc(c);
			bus_read(c, c->priv->operand);
			break;
		
		case 4:
			c->priv->scratch8 = data(c);
			bus_read(c, (c->priv->operand & 0xFF00) | ((c->priv->operand + 1) & 0xFF)); // Intentional bug
			break;
		
		case 5:
			c->priv->regs->pcl = c->priv->scratch8;
			c->priv->regs->pch = data(c);
			c->priv->cycle = 0;
			break;
			
		default:
			fprintf(stderr, "illegal cycle count\n");
			
		}
		break;
	
	case MODE_INDX:
		switch (c->priv->cycle) {
		case 2:
			c->priv->operand = data(c);
			pc_inc(c);
			bus_read(c, c->priv->operand);
			break;
		
		case 3:
			c->priv->scratch16 = (c->priv->operand + c->priv->regs->x) & 0xFF;
			bus_read(c, c->priv->scratch16);
			break;
		
		case 4:
			c->priv->scratch8 = data(c);
			c->priv->scratch16 = (c->priv->scratch16 + 1) & 0xFF;
			bus_read(c, c->priv->scratch16);
			break;
		
		case 5:
			c->priv->scratch16 = (data(c) << 8) | c->priv->scratch8;
			switch (c->priv->g) {
			case GRP_R:
				bus_read(c, c->priv->scratch16);
				break;
			
			case GRP_W:
				bus_write(c, c->priv->scratch16, apply_write_op(c));
				break;
			
			case GRP_RMW:
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;
		
		case 6:
			switch (c->priv->g) {
			case GRP_R:
				apply_read_op(c, data(c));
				c->priv->cycle = 0;
				break;
			
			case GRP_W:
				c->priv->cycle = 0;
				break;
			
			case GRP_RMW:
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;
		
		default:
			fprintf(stderr, "illegal cycle count\n");
			
		}
		break;
	
	case MODE_INDY:
		switch (c->priv->cycle) {
		case 2:
			c->priv->operand = data(c);
			pc_inc(c);
			bus_read(c, c->priv->operand);
			break;
		
		case 3:
			c->priv->scratch16 = data(c);
			bus_read(c, (c->priv->operand + 1) & 0xFF);
			break;
		
		case 4:
			c->priv->scratch8 = data(c);
			c->priv->scratch16 += c->priv->regs->y;
			c->priv->scratch1 = (c->priv->scratch16 <= 0xFF);
			c->priv->scratch16 = c->priv->scratch16 | ((u16)c->priv->scratch8 << 8);
			bus_read(c, c->priv->scratch16);
			break;
		
		case 5:
			if ((c->priv->g == GRP_R) && (c->priv->scratch1)) {
				apply_read_op(c, data(c));
				c->priv->cycle = 0;
				break;
			} else if (!c->priv->scratch1) {
				c->priv->scratch16 += 0x100;
			}

			switch (c->priv->g) {
			case GRP_R:
				bus_read(c, c->priv->scratch16);
				break;
			
			case GRP_W:
				bus_write(c, c->priv->scratch16, apply_write_op(c));
				break;
			
			case GRP_RMW:
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;
		
		case 6:
			switch (c->priv->g) {
			case GRP_R:
				apply_read_op(c, data(c));
				c->priv->cycle = 0;
				break;
			
			case GRP_W:
				c->priv->cycle = 0;
				break;
			
			case GRP_RMW:
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;
		
		default:
			fprintf(stderr, "illegal cycle count\n");
			
		}
		break;
	
	case MODE_REL:
		switch (c->priv->cycle) {
		case 2:
			c->priv->operand = data(c);
			pc_inc(c);
			if (!branch_taken(c)) {
				c->priv->cycle = 0;
			}
			bus_read(c, PC);
			break;
		
		case 3:
			c->priv->scratch16 = c->priv->regs->pcl + (s8)c->priv->operand;
			c->priv->regs->pcl += (s8)c->priv->operand;
			if (c->priv->scratch16 <= 0xFF) {
				c->priv->cycle = 0;
			}
			bus_read(c, PC);
			break;
		
		case 4:
			if (c->priv->scratch16 <= 0x17E) {
				c->priv->regs->pch++;
			} else {
				c->priv->regs->pch--;
			}
			c->priv->cycle = 0;
			break;

		default:
			fprintf(stderr, "illegal cycle count\n");
			
		}
		break;
	
	case MODE_ZPG:
		switch (c->priv->cycle) {
		case 2:
			c->priv->operand = data(c);
			pc_inc(c);
			switch (c->priv->g) {
			case GRP_R:
			case GRP_RMW:
				bus_read(c, c->priv->operand);
				break;
			
			case GRP_W:
				bus_write(c, c->priv->operand, apply_write_op(c));
				break;
			
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;

		case 3:
			switch (c->priv->g) {
			case GRP_R:
				apply_read_op(c, data(c));
				c->priv->cycle = 0;
				break;
			
			case GRP_W:
				c->priv->cycle = 0;
				break;
			
			case GRP_RMW:
				c->priv->scratch8 = data(c);
				bus_write(c, c->priv->operand, c->priv->scratch8);
				break;
			
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;

		case 4:
			c->priv->scratch8 = apply_rmw_op(c, c->priv->scratch8);
			bus_write(c, c->priv->operand, c->priv->scratch8);
			break;

		case 5:
			c->priv->cycle = 0;
			break;
		
		default:
			fprintf(stderr, "illegal cycle count\n");
			
		}
		break;
	
	case MODE_ZPGX:
		switch (c->priv->cycle) {
		case 2:
			c->priv->operand = data(c);
			pc_inc(c);
			bus_read(c, c->priv->operand);
			break;
		
		case 3:
			c->priv->scratch16 = (u8)(c->priv->operand + c->priv->regs->x);
			switch (c->priv->g) {
			case GRP_R:
			case GRP_RMW:
				bus_read(c, c->priv->scratch16);
				break;
			
			case GRP_W:
				bus_write(c, c->priv->scratch16, apply_write_op(c));
				break;
			
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;

		case 4:
			switch (c->priv->g) {
			case GRP_R:
				apply_read_op(c, data(c));
				c->priv->cycle = 0;
				break;
			
			case GRP_W:
				c->priv->cycle = 0;
				break;
			
			case GRP_RMW:
				c->priv->scratch8 = data(c);
				bus_write(c, c->priv->scratch16, c->priv->scratch8);
				break;
			
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;

		case 5:
			c->priv->scratch8 = apply_rmw_op(c, c->priv->scratch8);
			bus_write(c, c->priv->scratch16, c->priv->scratch8);
			break;

		case 6:
			c->priv->cycle = 0;
			break;
		
		default:
			fprintf(stderr, "illegal cycle count\n");
			
		}
		break;
	
	case MODE_ZPGY:
		switch (c->priv->cycle) {
		case 2:
			c->priv->operand = data(c);
			pc_inc(c);
			bus_read(c, c->priv->operand);
			break;
		
		case 3:
			c->priv->scratch16 = (u8)(c->priv->operand + c->priv->regs->y);
			switch (c->priv->g) {
			case GRP_R:
				bus_read(c, c->priv->scratch16);
				break;
			
			case GRP_W:
				bus_write(c, c->priv->scratch16, apply_write_op(c));
				break;
			
			case GRP_RMW:
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;

		case 4:
			switch (c->priv->g) {
			case GRP_R:
				apply_read_op(c, data(c));
				c->priv->cycle = 0;
				break;
			
			case GRP_W:
				c->priv->cycle = 0;
				break;
			
			case GRP_RMW:
			case GRP_NONE:
			case GRP_ILLEGAL:
			default:
				fprintf(stderr, "attempted illegal operation\n");
				
			}
			break;
		
		default:
			fprintf(stderr, "illegal cycle count\n");
			
		}
		break;
	
	case MODE_ILLEGAL:
	default:
		fprintf(stderr, "attempted illegal operation\n");
		
	}

tick_end:
	if (*c->nmi && !c->priv->nmi_prev) {
		c->priv->nmi_pending = true;
	}
	c->priv->nmi_prev = *c->nmi;

	c->priv->nmi_take = c->priv->nmi_now;
	c->priv->nmi_now  = c->priv->nmi_pending;


	c->priv->irq_take = c->priv->irq_now;
	c->priv->irq_now = (*c->irq && !GET_FLAG(FLAG_I));

	if (!c->priv->cycle) {
		bus_read(c, PC);
		if (t) {
			print_instruction(c);
		}
		if (c->priv->nmi_running) {
			c->priv->nmi_pending = false;
			c->priv->nmi_now = false;
			c->priv->nmi_take = false;
		}
		c->priv->irq_running = false;
		c->priv->nmi_running = false;
		c->priv->reset_running = false;
	}
	c->priv->cycle++;
}
