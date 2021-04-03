#ifndef Z80_H
#define Z80_H
#include "main.h"
typedef uint32_t u32;
typedef uint8_t u8;
typedef uint16_t u16;
typedef int32_t s32;
typedef int8_t s8;
typedef int16_t s16;

typedef union 
{
	u8   r[22];//C, B, E, D, L, H, F, A, IXL, IXH, IYL, IYH, SPL, SPH, PCL, PCH, MEMPTRL, MEMPTRH, I, R, R7,dummy
	u16 rp[11];//BC, DE, HL, AF, IX, IY, SP, PC, MEMPTR ,
}ZReg;

typedef union 
{
	u8 r[8];//C, B, E, D, L, H, F, A
	u16	rp[4];//BC, DE, HL, AF
}ZReg_Shadow;

//~ typedef struct
//~ {
	//~ unsigned  r_IM:2;
	//~ unsigned r_IFF1:1;
	//~ unsigned r_IFF2:1;
	//~ unsigned r_halt:1;
	//~ unsigned r_screen_IRQ:1;
//~ }ZINTRFlags;
typedef struct
{
	u8  r_IM;
	u8 r_IFF1;
	u8 r_IFF2;
	u8 r_halt;
	u8 r_screen_IRQ;
}ZINTRFlags;

extern ZReg reg;
extern ZReg_Shadow reg_;
extern ZINTRFlags IFlags;
#define   IM    IFlags.r_IM
#define   IFF1 IFlags.r_IFF1
#define   IFF2 IFlags.r_IFF2
#define   halt  IFlags.r_halt
#define   screen_IRQ  IFlags.r_screen_IRQ


#define RC(arg)   (reg.r[(arg)^1])
/*macros used for accessing the registers*/
#define A	   reg.r[6]
#define F     reg.r[21]
//~ #define FA  reg.rp[3]

#define B   reg.r[1]
#define C   reg.r[0]
#define BC  reg.rp[0]

#define D   reg.r[3]
#define E   reg.r[2]
#define DE  reg.rp[1]

#define H   reg.r[5]
#define L   reg.r[4]
#define HL  reg.rp[2]

#define IXH reg.r[9]
#define IXL reg.r[8]
#define IX  reg.rp[4]

#define IYH reg.r[11]
#define IYL reg.r[10]
#define IY  reg.rp[5]

#define SPH reg.r[13]
#define SPL reg.r[12]
#define SP  reg.rp[6]

#define PCH reg.r[15]
#define PCL reg.r[14]
#define PC  reg.rp[7]

#define MEMPTRH reg.r[17]
#define MEMPTRL reg.r[16]
#define MEMPTR reg.rp[8]

#define I  reg.r[18]
#define R  reg.r[19]
#define R7  reg.r[20]

#define A_  reg_.r[7]
#define F_  reg_.r[6]
#define AF_ reg_.rp[3]

#define B_  reg_.r[1]
#define C_  reg_.r[0]
#define BC_ reg_.rp[0]

#define D_  reg_.r[3]
#define E_  reg_.r[2]
#define DE_ reg_.rp[1]

#define H_  reg_.r[5]
#define L_  reg_.r[4]
#define HL_ reg_.rp[2]

/*the flags*/
#define FLAG_C	0x01
#define FLAG_N	0x02
#define FLAG_P	0x04
#define FLAG_V	FLAG_P
#define FLAG_3	0x08
#define FLAG_H	0x10
#define FLAG_5	0x20
#define FLAG_Z	0x40
#define FLAG_S	0x80

/*macros R/W memory*/
#define NEXT_WORD peek16(PC); PC=PC+2;
#define NEXT_BYTE peek(PC); PC++;

#define READ_WORD(addr) peek16(addr);
#define WRITE_WORD(addr, value) poke16(addr, value);
#define READ_BYTE(addr) (peek(addr))
#define WRITE_BYTE(addr, value) poke(addr, value);

/*macros R/W port*/
#define READ_PORT(port) in(port);
#define WRITE_PORT(port, value) out(port, value);

/*macros timing*/
#define T_WAIT_UNTIL(value) m_cycle=value;
#define T_WAIT_ADD(value) m_cycle+=value;
//~ //63428
//~ #define T_WAIT_UNTIL(value);
//~ #define T_WAIT_ADD(value);
//54900
/*instructions LOAD*/
#define LD(dst, src) \
{ \
	dst=src; \
}

#define LD16(dst, src) \
{ \
	dst=src; \
}

/*ld a, (BC|DE|nnnn)*/
#define LD_A_FROM_ADDR_MPTR(dst, src, addr) \
{ \
	dst=src; \
	MEMPTR=addr+1; \
}

/*ld (nnnn|BC|DE), A*/
#define LD_A_TO_ADDR_MPTR(dst, src, addr) \
{ \
	dst=src; \
	MEMPTRH=A; \
	MEMPTRL=((addr+1) & 0xFF); \
}

/*ld BC|DE|SP|HL|IX|IY, (nnnn)*/
#define LD_RP_FROM_ADDR_MPTR_16(dst, src, addr) \
{ \
	dst=src; \
	MEMPTR=addr+1; \
}

/*ld (nnnn), BC|DE|SP|HL|IX|IY*/
#define LD_RP_TO_ADDR_MPTR_16(dst, src, addr) \
{ \
	dst=src; \
	MEMPTR=addr+1; \
}

#define LD_A_I() \
{ \
	A=I; \
	F=(F&FLAG_C)|sz53_table[A]|(IFF2?FLAG_V:0); \
}

#define LD_A_R() \
{ \
	A=(R&0x7f)|(R7&0x80); \
	F=(F&FLAG_C)|sz53_table[A]|(IFF2?FLAG_V:0); \
}

#define LD_R_A() \
{ \
	R=R7=A; \
}

/*instructions EXCHANGE*/
#define EXX() \
{ \
	u16 exx_temp=BC; BC=BC_; BC_=exx_temp; \
	exx_temp=DE; DE=DE_; DE_=exx_temp; \
	exx_temp=HL; HL=HL_; HL_=exx_temp; \
}

#define EX(rp1, rp2) \
{ \
	u16 exx_temp=rp1; rp1=rp2; rp2=exx_temp; \
}

#define EX_MPTR(rp1, rp2) \
{ \
	u16 exx_temp=rp1; rp1=rp2; rp2=exx_temp; \
	MEMPTR=exx_temp; \
}

/*instructions ALU*/
#define ADD(a, value) \
{ \
	u32 add_temp=A+(value); \
	lookup=((A&0x88 )>>3 )|(((value)&0x88)>>2)|((add_temp&0x88)>>1);  \
	A=add_temp; \
	F=(add_temp&0x100?FLAG_C:0)|halfcarry_add_table[lookup&0x07]|overflow_add_table[lookup>>4]|sz53_table[A]; \
}

#define SUB(value) \
{ \
	u32 sub_temp=A-(value); \
	lookup=((A&0x88)>>3)|(((value)&0x88)>>2)|((sub_temp&0x88)>>1); \
	A=sub_temp; \
	F=(sub_temp&0x100?FLAG_C:0)|FLAG_N|halfcarry_sub_table[lookup&0x07]|overflow_sub_table[lookup>>4]|sz53_table[A]; \
}

#define ADD16(value1, value2) \
{ \
	u32 add_temp=(value1)+(value2); \
	lookup=(((value1)&0x0800)>>11)|(((value2)&0x0800)>>10)|((add_temp&0x0800)>>9 );  \
	MEMPTR=value1+1; \
	(value1)=add_temp; \
	F=(F&(FLAG_V|FLAG_Z|FLAG_S))|(add_temp&0x10000?FLAG_C:0)|((add_temp>>8)&(FLAG_3|FLAG_5))|halfcarry_add_table[lookup]; \
}

#define INC(value) \
{ \
	(value)++; \
	F=(F&FLAG_C)|((value)==0x80?FLAG_V:0)|((value)&0x0f?0:FLAG_H)|sz53_table[(value)]; \
}

#define DEC(value) \
{ \
	F=(F&FLAG_C)|((value)&0x0f?0:FLAG_H)|FLAG_N; \
	(value)--; \
	F|=((value)==0x7f?FLAG_V:0)|sz53_table[(value)]; \
}

#define INC16(value) \
{ \
	(value)++; \
}

#define DEC16(value) \
{ \
	(value)--; \
}

#define ADDC(a, value) \
{ \
	u32 add_temp=A+(value)+(F&FLAG_C); \
	lookup=((A&0x88)>>3)|(((value)&0x88)>>2 )|((add_temp&0x88)>>1); \
	A=add_temp; \
	F=(add_temp&0x100?FLAG_C:0)|halfcarry_add_table[lookup & 0x07]|overflow_add_table[lookup>>4]|sz53_table[A]; \
}

#define ADDC16(hl, value) \
{ \
	u32 add_temp=HL+(value)+(F&FLAG_C); \
	lookup=((HL&0x8800)>>11)|(((value)&0x8800)>>10)|((add_temp&0x8800)>>9); \
	MEMPTR=hl+1; \
	HL=add_temp; \
	F=(add_temp&0x10000?FLAG_C:0)|overflow_add_table[lookup>>4]|(H&(FLAG_3|FLAG_5|FLAG_S))|halfcarry_add_table[lookup&0x07]|(HL?0:FLAG_Z); \
}

#define SUBC(a, value) \
{ \
	u32 sub_temp=A-(value)-(F&FLAG_C); \
	lookup=((A&0x88)>>3)|(((value)&0x88)>>2)|((sub_temp&0x88)>>1); \
	A=sub_temp;\
	F=(sub_temp&0x100?FLAG_C:0)|FLAG_N|halfcarry_sub_table[lookup&0x07]|overflow_sub_table[lookup>>4]|sz53_table[A]; \
}

#define SUBC16(hl, value) \
{ \
	u32 sub_temp=HL-(value)-(F&FLAG_C); \
	lookup=((HL&0x8800)>>11)|(((value)&0x8800)>>10)|((sub_temp&0x8800)>>9); \
	MEMPTR=hl+1; \
	HL=sub_temp; \
	F=(sub_temp&0x10000?FLAG_C:0)|FLAG_N|overflow_sub_table[lookup>>4]|(H&(FLAG_3|FLAG_5|FLAG_S))|halfcarry_sub_table[lookup&0x07]|(HL?0:FLAG_Z); \
}

#define CP(value) \
{ \
	u16 cp_temp=A-(value); \
	lookup=((A&0x88)>>3)|(((value)&0x88)>>2)|((cp_temp&0x88)>>1); \
	F=(cp_temp&0x100?FLAG_C:(cp_temp?0:FLAG_Z))|FLAG_N|halfcarry_sub_table[lookup&0x07]|overflow_sub_table[lookup>>4]|((value)&(FLAG_3|FLAG_5))|(cp_temp&FLAG_S); \
}

#define AND(value) \
{ \
	A&=(value); \
	F=FLAG_H|sz53p_table[A]; \
}

#define OR(value) \
{ \
	A|=(value); \
	F=sz53p_table[A]; \
}

#define XOR(value) \
{ \
	A^=(value); \
	F=sz53p_table[A]; \
}

/*instructions JAMP*/
#define JP(addr) \
{ \
	PC=addr; \
	MEMPTR=addr; \
}

#define JP_NO_MPTR(addr) \
{ \
	PC=addr; \
}

#define JR(offset) \
{ \
	PC+=offset; \
	MEMPTR=PC; \
}

/*instructions STACK*/
#define PUSH(rp) \
{ \
	SP-=2; \
	WRITE_WORD(SP, rp); \
}

#define POP(rp) \
{ \
	rp=READ_WORD(SP); \
	SP+=2; \
}

#define RST(addr) \
{ \
	PUSH(PC); \
	PC=addr; \
	MEMPTR=PC; \
}

#define CALL(addr) \
{ \
	PUSH(PC); \
	PC=addr; \
	MEMPTR=addr; \
}

#define RET() \
{ \
	POP(PC); \
	MEMPTR=PC; \
}

/*instructions LOAD BLOCK*/
#define LDI() \
{ \
	u8 bo_temp=READ_BYTE(HL); \
	WRITE_BYTE(DE, bo_temp); \
	BC--; \
	DE++; \
	HL++; \
	bo_temp+=A; \
	F=(F&(FLAG_C|FLAG_Z|FLAG_S))|(BC?FLAG_V:0)|(bo_temp&FLAG_3)|((bo_temp&0x02)?FLAG_5:0); \
}

#define LDD() \
{ \
	u8 bo_temp=READ_BYTE(HL); \
	WRITE_BYTE(DE, bo_temp); \
	BC--; \
	DE--; \
	HL--; \
	bo_temp+=A; \
	F=(F&(FLAG_C|FLAG_Z|FLAG_S))|(BC?FLAG_V:0)|(bo_temp&FLAG_3)|((bo_temp&0x02)?FLAG_5:0); \
}

#define LDIR() \
{ \
	u8 bo_temp=READ_BYTE(HL); \
	WRITE_BYTE(DE, bo_temp); \
	BC--; \
	DE++; \
	HL++; \
	bo_temp+=A; \
	F=(F&(FLAG_C|FLAG_Z|FLAG_S))|(BC?FLAG_V:0)|(bo_temp&FLAG_3)|((bo_temp&0x02)?FLAG_5:0); \
	if(BC) \
	{ \
		PC-=2; \
		T_WAIT_UNTIL(17); \
		MEMPTR=PC+1; \
	} \
	else \
	{ \
		T_WAIT_UNTIL(12); \
	} \
}

#define LDDR() \
{ \
	u8 bo_temp=READ_BYTE(HL); \
	WRITE_BYTE(DE, bo_temp); \
	BC--; \
	DE--; \
	HL--; \
	bo_temp+=A; \
	F=(F&(FLAG_C|FLAG_Z|FLAG_S))|(BC?FLAG_V:0)|(bo_temp&FLAG_3)|((bo_temp&0x02)?FLAG_5:0); \
	if(BC) \
	{ \
		PC-=2; \
		T_WAIT_UNTIL(17); \
		MEMPTR=PC+1; \
	} \
	else \
	{ \
		T_WAIT_UNTIL(12); \
	} \
}

/*instructions COMPARE BLOCK*/
#define CPI(rd) \
{ \
	u8 bo_temp=READ_BYTE(HL); \
	u16 cp_temp=A-bo_temp; \
	lookup=((A&0x08)>>3)|((bo_temp&0x08 )>>2)|((cp_temp&0x08)>>1); \
	HL++; \
	BC--; \
	F=(F&FLAG_C)|(BC?(FLAG_V|FLAG_N):FLAG_N)|halfcarry_sub_table[lookup]|(cp_temp?0:FLAG_Z)|(cp_temp&FLAG_S ); \
	if(F&FLAG_H) \
	{ \
		cp_temp--; \
	} \
	F|=(cp_temp&FLAG_3)|((cp_temp&0x02)?FLAG_5:0); \
	MEMPTR=MEMPTR+1; \
}

#define CPD(rd) \
{ \
	u8 bo_temp=READ_BYTE(HL); \
	u16 cp_temp=A-bo_temp; \
	lookup=((A&0x08)>>3)|((bo_temp&0x08 )>>2)|((cp_temp&0x08)>>1); \
	HL--; \
	BC--; \
	F=(F&FLAG_C)|(BC?(FLAG_V|FLAG_N):FLAG_N)|halfcarry_sub_table[lookup]|(cp_temp?0:FLAG_Z)|(cp_temp&FLAG_S ); \
	if(F&FLAG_H) \
	{ \
		cp_temp--; \
	} \
	F|=(cp_temp&FLAG_3)|((cp_temp&0x02)?FLAG_5:0); \
	MEMPTR=MEMPTR-1; \
}

#define CPIR() \
{ \
	u8 bo_temp=READ_BYTE(HL); \
	u16 cp_temp=A-bo_temp; \
	lookup=((A&0x08)>>3)|((bo_temp&0x08 )>>2)|((cp_temp&0x08)>>1); \
	HL++; \
	BC--; \
	F=(F&FLAG_C)|(BC?(FLAG_V|FLAG_N):FLAG_N)|halfcarry_sub_table[lookup]|(cp_temp?0:FLAG_Z)|(cp_temp&FLAG_S ); \
	if(F&FLAG_H) \
	{ \
		cp_temp--; \
	} \
	F|=(cp_temp&FLAG_3)|((cp_temp&0x02)?FLAG_5:0); \
	if((F&(FLAG_V|FLAG_Z))==FLAG_V) \
	{ \
		PC-=2; \
		MEMPTR=PC+1; \
		T_WAIT_UNTIL(17); \
	} \
	else \
	{ \
		MEMPTR=MEMPTR+1; \
		T_WAIT_UNTIL(12); \
	} \
}

#define CPDR() \
{ \
	u8 bo_temp=READ_BYTE(HL); \
	u16 cp_temp=A-bo_temp; \
	lookup=((A&0x08)>>3)|((bo_temp&0x08 )>>2)|((cp_temp&0x08)>>1); \
	HL--; \
	BC--; \
	F=(F&FLAG_C)|(BC?(FLAG_V|FLAG_N):FLAG_N)|halfcarry_sub_table[lookup]|(cp_temp?0:FLAG_Z)|(cp_temp&FLAG_S ); \
	if(F&FLAG_H) \
	{ \
		cp_temp--; \
	} \
	F|=(cp_temp&FLAG_3)|((cp_temp&0x02)?FLAG_5:0); \
	if((F&(FLAG_V|FLAG_Z))==FLAG_V) \
	{ \
		PC-=2; \
		MEMPTR=PC+1; \
		T_WAIT_UNTIL(17); \
	} \
	else \
	{ \
		MEMPTR=MEMPTR-1; \
		T_WAIT_UNTIL(12); \
	} \
}


/*instructions IN PORT*/
#define IN_A(reg, port) \
{ \
	reg=READ_PORT(port); \
	MEMPTR=port+1; \
}

#define IN(reg, port) \
{ \
	MEMPTR=port+1; \
	reg=READ_PORT(port); \
	F=(F&FLAG_C)|sz53p_table[(reg)]; \
}

#define IN_F(port) \
{ \
	u8 in_temp=READ_PORT(port); \
	F=(F& FLAG_C)|sz53p_table[in_temp]; \
	MEMPTR=port+1; \
}

#define INI() \
{ \
	MEMPTR=BC+1; \
	u8 bo_temp=READ_PORT(BC); \
	WRITE_BYTE(HL, bo_temp); \
	B--; \
	HL++; \
	F=(bo_temp&0x80?FLAG_N:0)|sz53_table[B]; \
	IN_BL(bo_temp, 1); \
}

#define IND() \
{ \
	MEMPTR=BC-1; \
	u8 bo_temp=READ_PORT(BC); \
	WRITE_BYTE(HL, bo_temp); \
	B--; \
	HL--; \
	F=(bo_temp&0x80?FLAG_N:0 )|sz53_table[B]; \
	IN_BL(bo_temp, -1); \
}

#define INIR() \
{ \
	u8 bo_temp=READ_PORT(BC); \
	WRITE_BYTE(HL, bo_temp); \
	MEMPTR=BC+1; \
	B--; \
	HL++; \
	F=(bo_temp&0x80?FLAG_N:0)|sz53_table[B]; \
	if(B) \
	{ \
		PC -= 2; \
		T_WAIT_UNTIL(17); \
	} \
	else \
	{ \
		T_WAIT_UNTIL(12); \
	} \
	IN_BL(bo_temp, 1); \
}

#define INDR() \
{ \
	u8 bo_temp=READ_PORT(BC); \
	WRITE_BYTE(HL, bo_temp); \
	MEMPTR=BC-1;\
	B--; \
	HL--; \
	F=(bo_temp&0x80?FLAG_N:0)|sz53_table[B]; \
	if(B) \
	{ \
		PC -= 2; \
		T_WAIT_UNTIL(17); \
	} \
	else \
	{ \
		T_WAIT_UNTIL(12); \
	} \
	IN_BL(bo_temp, -1); \
}

/*undocumented flag effects for block input operations*/
#define IN_BL(pbyte, c_add) \
{ \
	if((pbyte+((C+(c_add))&0xff))>255) F|=(FLAG_C|FLAG_H); \
	F|=(1-__builtin_parity((((pbyte+((C+(c_add))&0xff))&7)^B)))*4; \
}


/*instructions OUT PORT*/
#define OUT_A(port, reg) \
{ \
	WRITE_PORT(port, reg); \
	MEMPTRL=((port+1)&0xFF); \
	MEMPTRH=A; \
}

#define OUT(port, reg) \
{ \
	WRITE_PORT(port, reg); \
	MEMPTR=port+1; \
}

#define OUTI() \
{ \
	u8 bo_temp=READ_BYTE(HL); \
	B--; \
	MEMPTR=BC+1; \
	WRITE_PORT(BC, bo_temp); \
	HL++; \
	F=(bo_temp&0x80?FLAG_N:0)|sz53_table[B]; \
	OUT_BL(bo_temp); \
}

#define OUTD() \
{ \
	u8 bo_temp=READ_BYTE(HL); \
	B--; \
	MEMPTR=BC-1; \
	WRITE_PORT(BC, bo_temp); \
	HL--; \
	F= (bo_temp&0x80?FLAG_N:0 )|sz53_table[B]; \
	OUT_BL(bo_temp); \
}

#define OTIR() \
{ \
	u8 bo_temp=READ_BYTE(HL); \
	B--; \
	MEMPTR=BC+1; \
	WRITE_PORT(BC, bo_temp); \
	HL++; \
	F=(bo_temp&0x80?FLAG_N:0)|sz53_table[B]; \
	if(B) \
	{ \
		PC-= 2; \
		T_WAIT_UNTIL(17); \
	} \
	else \
	{ \
		T_WAIT_UNTIL(12);\
	} \
	OUT_BL(bo_temp); \
}

#define OTDR() \
{ \
	u8 bo_temp=READ_BYTE(HL); \
	B--; \
	MEMPTR=BC-1; \
	WRITE_PORT(BC, bo_temp); \
	HL--; \
	F=(bo_temp&0x80?FLAG_N:0)|sz53_table[B]; \
	if(B) \
	{ \
		PC-= 2; \
		T_WAIT_UNTIL(17); \
	} \
	else \
	{ \
		T_WAIT_UNTIL(12); \
	} \
	OUT_BL(bo_temp); \
}

/*undocumented flag effects for block output operations*/
#define OUT_BL(pbyte) \
{ \
	if((pbyte+L)>255) F|=(FLAG_C|FLAG_H); \
	F|=(1-__builtin_parity((((pbyte+L)&7)^B)))*4; \
}

/*instructions OTHE*/
#define DI() \
{ \
	IFF1=IFF2=0; \
}

#define EI() \
{ \
	IFF1=IFF2=1; \
}

#define IM_MODE(value) \
{ \
	IM=(value); \
}

/*same as RETI, only opcode is different*/
#define RETN() \
{ \
	IFF1=IFF2; \
	RET(); \
}

#define RETI() \
{ \
	IFF1=IFF2; \
	RET(); \
}

#define CPL() \
{ \
	A^= 0xff; \
	F=(F&(FLAG_C|FLAG_P|FLAG_Z|FLAG_S))|(A&(FLAG_3|FLAG_5))|(FLAG_N|FLAG_H); \
}

#define NEG() \
{ \
	u8 neg_temp=A; \
	A=0; \
	SUB(neg_temp); \
}

#define SCF() \
{ \
	F=(F&(FLAG_P|FLAG_Z|FLAG_S))|(A&(FLAG_3|FLAG_5))|FLAG_C; \
}

#define CCF() \
{ \
	F=(F&(FLAG_P|FLAG_Z|FLAG_S))|((F&FLAG_C)?FLAG_H:FLAG_C)|(A&(FLAG_3|FLAG_5)); \
}

#define HALT() \
{ \
	halt=1; \
}

/*instructions ROT*/
#define SRL(value) \
{ \
	F=(value)&FLAG_C; \
	(value)>>=1; \
	F|=sz53p_table[(value)]; \
}

#define SRA(value) \
{ \
	F=(value)&FLAG_C; \
	(value)=((value)&0x80)|((value)>>1); \
	F|=sz53p_table[(value)]; \
}

#define SLA(value) \
{ \
	F=(value)>>7; \
	(value)<<=1; \
	F|=sz53p_table[(value)]; \
}

#define SLL(value) \
{ \
	F=(value)>>7; \
	(value)=((value)<<1)|0x01; \
	F|=sz53p_table[(value)]; \
}

#define RL(value) \
{ \
	u8 rot_temp=(value); \
	(value)=((value)<<1)|( F & FLAG_C ); \
	F=(rot_temp>>7)|sz53p_table[(value)]; \
}

#define RR(value) \
{ \
	u8 rot_temp=(value); \
	(value)=((value)>>1)|(F<<7); \
	F=(rot_temp&FLAG_C)|sz53p_table[(value)]; \
}

#define RLC(value) \
{ \
	(value)=((value)<<1)|((value)>>7); \
	F=((value)&FLAG_C)|sz53p_table[(value)]; \
}

#define RRC(value) \
{ \
	F=(value)&FLAG_C; \
	(value)=((value)>>1)|((value)<<7); \
	F|=sz53p_table[(value)]; \
}

#define RLA() \
{ \
	u8 rot_temp=A; \
	A=(A<<1)|(F&FLAG_C); \
	F=(F&(FLAG_P|FLAG_Z|FLAG_S))|(A&(FLAG_3|FLAG_5))|(rot_temp>>7); \
}

#define RRA() \
{ \
	u8 rot_temp=A; \
	A=(A>>1)|(F<<7); \
	F=(F&(FLAG_P|FLAG_Z|FLAG_S))|(A&(FLAG_3|FLAG_5))|(rot_temp&FLAG_C); \
}

#define RLCA() \
{ \
	A=(A<<1)|(A>>7); \
	F=(F&(FLAG_P|FLAG_Z|FLAG_S))|(A&(FLAG_C|FLAG_3|FLAG_5)); \
}

#define RRCA() \
{ \
	F=(F&(FLAG_P|FLAG_Z|FLAG_S))|(A&FLAG_C); \
	A=(A>>1)|(A<<7); \
	F|=(A&(FLAG_3|FLAG_5)); \
}

#define RRD() \
{ \
	u8 rot_temp=READ_BYTE(HL); \
	WRITE_BYTE(HL, (A<<4)|(rot_temp>>4)); \
	A=(A&0xf0)|(rot_temp&0x0f); \
	F=(F&FLAG_C)|sz53p_table[A]; \
	MEMPTR=HL+1; \
}

#define RLD() \
{ \
	u8 rot_temp=READ_BYTE(HL); \
	WRITE_BYTE(HL, (rot_temp<<4)|(A&0x0f)); \
	A=(A&0xf0)|(rot_temp>>4); \
	F=(F&FLAG_C)|sz53p_table[A]; \
	MEMPTR=HL+1; \
}

/*instructions BIT*/
#define BIT(bit, value) \
{ \
	F=(F&FLAG_C)|FLAG_H|sz53p_table[(value)&(0x01<<(bit))]|((value)&0x28); \
}

/*BIT n,(IX+d/IY+d) and BIT n,(HL)*/
#define BIT_MPTR(bit, value) \
{ \
	F=(F&FLAG_C)|FLAG_H|(sz53p_table[(value)&(0x01<<(bit))]&0xD7)|((MEMPTRH)&0x28); \
}

#define SET(bit, value) \
{ \
	value|=(1<<bit); \
}

#define RES(bit, value) \
{ \
	value&=~(1<<bit); \
}

//void z80_reset(int hard);
void z80_run(int num_reps);
void z80_reset( int hard_reset );


void poke(u16 addr, u8 value);
u8 peek(u16 addr);
void poke16(u16 addr, u16 value);
u16 peek16(u16 addr);

u8 in(u16 port);
void out(u16 port, u8 value);


#endif
