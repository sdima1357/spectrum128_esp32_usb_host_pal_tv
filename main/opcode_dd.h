//~ void opcode_dd(u8 opcode)
#undef IX
#undef IXH 
#undef IXL
#define IX (pntReg[0])
#define IXL (((u8*)pntReg)[0])
#define IXH (((u8*)pntReg)[1]) 
{
switch (prefix) 
{
/*ADD IX, BC*/
case 0x09:
	ADD16(IX, BC);
	T_WAIT_UNTIL(-4+11);
break;

/*ADD IX, DE*/
case 0x19:
	ADD16(IX, DE);
	T_WAIT_UNTIL(-4+11);
break;

/*LD IX, @*/
case 0x21:
	nn=NEXT_WORD;
	LD16(IX, nn);
	T_WAIT_UNTIL(-4+10);
break;

/*LD (@), IX*/
case 0x22:
	nn=NEXT_WORD;
	LD_RP_TO_ADDR_MPTR_16(temp16, IX, nn);
	WRITE_WORD(nn, temp16);
	T_WAIT_UNTIL(-4+16);
break;

/*INC IX*/
case 0x23:
	INC16(IX);
	T_WAIT_UNTIL(-4+6);
break;

/*INC IXH*/
case 0x24:
	INC(IXH);
	EMPTY;
break;

/*DEC IXH*/
case 0x25:
	DEC(IXH);
	EMPTY;
break;

/*LD IXH, #*/
case 0x26:
	n=NEXT_BYTE;
	LD(IXH, n);
	T_WAIT_UNTIL(-4+7);
break;

/*ADD IX, IX*/
case 0x29:
	ADD16(IX, IX);
	T_WAIT_UNTIL(-4+11);
break;

/*LD IX, (@)*/
case 0x2a:
	nn=NEXT_WORD;
	temp16=READ_WORD(nn);
	LD_RP_FROM_ADDR_MPTR_16(IX, temp16, nn);
	T_WAIT_UNTIL(-4+16);
break;

/*DEC IX*/
case 0x2b:
	DEC16(IX);
	T_WAIT_UNTIL(-4+6);
break;

/*INC IXL*/
case 0x2c:
	INC(IXL);
	EMPTY;
break;

/*DEC IXL*/
case 0x2d:
	DEC(IXL);
	EMPTY;
break;

/*LD IXL, #*/
case 0x2e:
	n=NEXT_BYTE;
	LD(IXL, n);
	T_WAIT_UNTIL(-4+7);
break;

/*INC (IX+$)*/
case 0x34:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	INC(temp8);
	WRITE_BYTE(IX+d.s, temp8);
	T_WAIT_UNTIL(-4+19);
break;

/*DEC (IX+$)*/
case 0x35:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	DEC(temp8);
	WRITE_BYTE(IX+d.s, temp8);
	T_WAIT_UNTIL(-4+19);
break;

/*LD (IX+$), #*/
case 0x36:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	n=NEXT_BYTE;
	LD(n, n);
	WRITE_BYTE(IX+d.s, n);
	T_WAIT_UNTIL(-4+15);
break;

/*ADD IX, SP*/
case 0x39:
	ADD16(IX, SP);
	T_WAIT_UNTIL(-4+11);
break;

/*LD B, IXH*/
case 0x44:
	LD(B, IXH);
	EMPTY;
break;

/*LD B, IXL*/
case 0x45:
	LD(B, IXL);
	EMPTY;
break;

/*LD B, (IX+$)*/
case 0x46:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	LD(B, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*LD C, IXH*/
case 0x4c:
	LD(C, IXH);
	EMPTY;
break;

/*LD C, IXL*/
case 0x4d:
	LD(C, IXL);
	EMPTY;
break;

/*LD C, (IX+$)*/
case 0x4e:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	LD(C, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*LD D, IXH*/
case 0x54:
	LD(D, IXH);
	EMPTY;
break;

/*LD D, IXL*/
case 0x55:
	LD(D, IXL);
	EMPTY;
break;

/*LD D, (IX+$)*/
case 0x56:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	LD(D, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*LD E, IXH*/
case 0x5c:
	LD(E, IXH);
	EMPTY;
break;

/*LD E, IXL*/
case 0x5d:
	LD(E, IXL);
	EMPTY;
break;

/*LD E, (IX+$)*/
case 0x5e:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	LD(E, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*LD IXH, B*/
case 0x60:
	LD(IXH, B);
	EMPTY;
break;

/*LD IXH, C*/
case 0x61:
	LD(IXH, C);
	EMPTY;
break;

/*LD IXH, D*/
case 0x62:
	LD(IXH, D);
	EMPTY;
break;

/*LD IXH, E*/
case 0x63:
	LD(IXH, E);
	EMPTY;
break;

/*LD IXH, IXH*/
case 0x64:
	LD(IXH, IXH);
	EMPTY;
break;

/*LD IXH, IXL*/
case 0x65:
	LD(IXH, IXL);
	EMPTY;
break;

/*LD H, (IX+$)*/
case 0x66:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	LD(H, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*LD IXH, A*/
case 0x67:
	LD(IXH, A);
	EMPTY;
break;

/*LD IXL, B*/
case 0x68:
	LD(IXL, B);
	EMPTY;
break;

/*LD IXL, C*/
case 0x69:
	LD(IXL, C);
	EMPTY;
break;

/*LD IXL, D*/
case 0x6a:
	LD(IXL, D);
	EMPTY;
break;

/*LD IXL, E*/
case 0x6b:
	LD(IXL, E);
	EMPTY;
break;

/*LD IXL, IXH*/
case 0x6c:
	LD(IXL, IXH);
	EMPTY;
break;

/*LD IXL, IXL*/
case 0x6d:
	LD(IXL, IXL);
	EMPTY;
break;

/*LD L, (IX+$)*/
case 0x6e:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	LD(L, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*LD IXL, A*/
case 0x6f:
	LD(IXL, A);
	EMPTY;
break;

/*LD (IX+$), B*/
case 0x70:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	LD(temp8, B);
	WRITE_BYTE(IX+d.s, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*LD (IX+$), C*/
case 0x71:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	LD(temp8, C);
	WRITE_BYTE(IX+d.s, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*LD (IX+$), D*/
case 0x72:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	LD(temp8, D);
	WRITE_BYTE(IX+d.s, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*LD (IX+$), E*/
case 0x73:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	LD(temp8, E);
	WRITE_BYTE(IX+d.s, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*LD (IX+$), H*/
case 0x74:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	LD(temp8, H);
	WRITE_BYTE(IX+d.s, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*LD (IX+$), L*/
case 0x75:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	LD(temp8, L);
	WRITE_BYTE(IX+d.s, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*LD (IX+$), A*/
case 0x77:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	LD(temp8, A);
	WRITE_BYTE(IX+d.s, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*LD A, IXH*/
case 0x7c:
	LD(A, IXH);
	EMPTY;
break;

/*LD A, IXL*/
case 0x7d:
	LD(A, IXL);
	EMPTY;
break;

/*LD A, (IX+$)*/
case 0x7e:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	LD(A, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*ADD A, IXH*/
case 0x84:
	ADD(A, IXH);
	EMPTY;
break;

/*ADD A, IXL*/
case 0x85:
	ADD(A, IXL);
	EMPTY;
break;

/*ADD A, (IX+$)*/
case 0x86:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	ADD(A, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*ADDC A, IXH*/
case 0x8c:
	ADDC(A, IXH);
	EMPTY;
break;

/*ADDC A, IXL*/
case 0x8d:
	ADDC(A, IXL);
	EMPTY;
break;

/*ADDC A, (IX+$)*/
case 0x8e:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	ADDC(A, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*SUB IXH*/
case 0x94:
	SUB(IXH);
	EMPTY;
break;

/*SUB IXL*/
case 0x95:
	SUB(IXL);
	EMPTY;
break;

/*SUB (IX+$)*/
case 0x96:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	SUB(temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*SUBC A, IXH*/
case 0x9c:
	SUBC(A, IXH);
	EMPTY;
break;

/*SUBC A, IXL*/
case 0x9d:
	SUBC(A, IXL);
	EMPTY;
break;

/*SUBC A, (IX+$)*/
case 0x9e:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	SUBC(A, temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*AND IXH*/
case 0xa4:
	AND(IXH);
	EMPTY;
break;

/*AND IXL*/
case 0xa5:
	AND(IXL);
	EMPTY;
break;

/*AND (IX+$)*/
case 0xa6:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	AND(temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*XOR IXH*/
case 0xac:
	XOR(IXH);
	EMPTY;
break;

/*XOR IXL*/
case 0xad:
	XOR(IXL);
	EMPTY;
break;

/*XOR (IX+$)*/
case 0xae:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	XOR(temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*OR IXH*/
case 0xb4:
	OR(IXH);
	EMPTY;
break;

/*OR IXL*/
case 0xb5:
	OR(IXL);
	EMPTY;
break;

/*OR (IX+$)*/
case 0xb6:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	OR(temp8);
	T_WAIT_UNTIL(-4+15);
break;

/*CP IXH*/
case 0xbc:
	CP(IXH);
	EMPTY;
break;

/*CP IXL*/
case 0xbd:
	CP(IXL);
	EMPTY;
break;

/*CP (IX+$)*/
case 0xbe:
	d.u=NEXT_BYTE
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
	CP(temp8);
	T_WAIT_UNTIL(-4+15);
break;

case 0xcb:
	d.u=NEXT_BYTE;
	prefix=NEXT_BYTE;
	MEMPTR=(IX+d.s);
	temp8=READ_BYTE(IX+d.s);
#include "opcode_ddcb.h"
	WRITE_BYTE(IX+d.s, temp8);
	m_cycle+=15;
	break;
/*POP IX*/
case 0xe1:
	POP(IX);
	T_WAIT_UNTIL(-4+10);
break;

/*EX (SP), IX*/
case 0xe3:
	temp16=READ_WORD(SP);
	EX_MPTR(temp16, IX);
	WRITE_WORD(SP, temp16);
	T_WAIT_UNTIL(-4+19);
break;

/*PUSH IX*/
case 0xe5:
	PUSH(IX);
	T_WAIT_UNTIL(-4+11);
break;

/*JP IX*/
case 0xe9:
	JP_NO_MPTR(IX);
	EMPTY;
break;

/*LD SP, IX*/
case 0xf9:
	LD16(SP, IX);
	T_WAIT_UNTIL(-4+6);
break;

/*NONI*/
default :
	PC--;
	R--;
	T_WAIT_UNTIL(-4);
	EMPTY;
break;
}
}
