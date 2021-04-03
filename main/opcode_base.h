//~ void opcode_base(u8 bt)
{
switch (opcode) 
{

/*NOP*/
case 0x00:
	T_WAIT_UNTIL(4);
break;
/*LD BC,@*/
case 0x01:
	nn=NEXT_WORD;
	LD16(BC, nn);
	T_WAIT_UNTIL(10);
break;
/*LD (BC), A*/
case 0x02:
	LD_A_TO_ADDR_MPTR(temp8, A, (BC));
	WRITE_BYTE((BC), temp8);
	T_WAIT_UNTIL(7);
break;
/*INC BC*/
case 0x03:
	INC16(BC);
	T_WAIT_UNTIL(6);
break;
/*INC B*/
case 0x04:
	INC(B);
	T_WAIT_UNTIL(4);
break;
/*DEC B*/
case 0x05:
	DEC(B);
	T_WAIT_UNTIL(4);
break;
/*LD B, #*/
case 0x06:
	n=NEXT_BYTE;
	LD(B, n);
	T_WAIT_UNTIL(7);
break;
/*RLCA*/
case 0x07:
	RLCA();
	T_WAIT_UNTIL(4);
break;
/*EX AF, AF'*/
case 0x08:
	EX(F, F_);
	EX(A, A_);
	T_WAIT_UNTIL(4);
break;
/*ADD HL, BC*/
case 0x09:
	ADD16(HL, BC);
	T_WAIT_UNTIL(11);
break;
/*LD A, (BC)*/
case 0x0a:
	temp8=READ_BYTE(BC);
	LD_A_FROM_ADDR_MPTR(A, temp8, BC);
	T_WAIT_UNTIL(7);
break;
/*DEC BC*/
case 0x0b:
	DEC16(BC);
	T_WAIT_UNTIL(6);
break;
/*INC C*/
case 0x0c:
	INC(C);
	T_WAIT_UNTIL(4);
break;
/*DEC C*/
case 0x0d:
	DEC(C);
	T_WAIT_UNTIL(4);
break;
/*LD C, #*/
case 0x0e:
	n=NEXT_BYTE;
	LD(C, n);
	T_WAIT_UNTIL(7);
break;
/*RRCA*/
case 0x0f:
	RRCA();
	T_WAIT_UNTIL(4);
break;
/*DJNZ %*/
case 0x10:
	d.u=NEXT_BYTE;
	B--;
	if(B)
	{
		JR(d.s);
		T_WAIT_UNTIL(13);
	}
	else
	{
		T_WAIT_UNTIL(8);
	}
break;
/*LD DE, @*/
case 0x11:
	nn=NEXT_WORD;
	LD16(DE, nn);
	T_WAIT_UNTIL(10);
break;
/*LD (DE), A*/
case 0x12:
	LD_A_TO_ADDR_MPTR(temp8, A, DE);
	WRITE_BYTE(DE, temp8);
	T_WAIT_UNTIL(7);
break;
/*INC DE*/
case 0x13:
	INC16(DE);
	T_WAIT_UNTIL(6);
break;
/*INC D*/
case 0x14:
	INC(D);
	T_WAIT_UNTIL(4);
break;
/*DEC D*/
case 0x15:
	DEC(D);
	T_WAIT_UNTIL(4);
break;
/*LD D, #*/
case 0x16:
	n=NEXT_BYTE;
	LD(D, n);
	T_WAIT_UNTIL(7);
break;
/*RLA*/
case 0x17:
	RLA();
	T_WAIT_UNTIL(4);
break;
/*JR %*/
case 0x18:
	d.u=NEXT_BYTE;
	JR(d.s);
	T_WAIT_UNTIL(12);
break;
/*ADD HL, DE*/
case 0x19:
	ADD16(HL, DE);
	T_WAIT_UNTIL(11);
break;
/*LD A,(DE)*/
case 0x1a:
	temp8=READ_BYTE(DE);
	LD_A_FROM_ADDR_MPTR(A, temp8, DE);
	T_WAIT_UNTIL(7);
break;
/*DEC DE*/
case 0x1b:
	DEC16(DE);
	T_WAIT_UNTIL(6);
break;
/*INC E*/
case 0x1c:
	INC(E);
	T_WAIT_UNTIL(4);
break;
/*DEC E*/
case 0x1d:
	DEC(E);
	T_WAIT_UNTIL(4);
break;
/*LD E, #*/
case 0x1e:
	n=NEXT_BYTE;
	LD(E, n);
	T_WAIT_UNTIL(7);
break;
/*RRA*/
case 0x1f:
	RRA();
	T_WAIT_UNTIL(4);
break;
/*JR NZ, %*/
case 0x20:
	d.u=NEXT_BYTE;
	if(!(F&FLAG_Z))
	{
		JR(d.s);
		T_WAIT_UNTIL(12);
	}
	else
	{
		T_WAIT_UNTIL(7);
	}
break;
/*LD HL, @*/
case 0x21:
	nn=NEXT_WORD;
	LD16(HL, nn);
	T_WAIT_UNTIL(10);
break;
/*LD (@), HL*/
case 0x22:
	nn=NEXT_WORD;
	LD_RP_TO_ADDR_MPTR_16(temp16, HL, nn);
	WRITE_WORD(nn, temp16);
	T_WAIT_UNTIL(16);
break;
/*INC HL*/
case 0x23:
	INC16(HL);
	T_WAIT_UNTIL(6);
break;
/*INC H*/
case 0x24:
	INC(H);
	T_WAIT_UNTIL(4);
break;
/*DEC H*/
case 0x25:
	DEC(H);
	T_WAIT_UNTIL(4);
break;
/*LD H, #*/
case 0x26:
	n=NEXT_BYTE;
	LD(H, n);
	T_WAIT_UNTIL(7);
break;
/*DAA*/
case 0x27:
      {
	u8 add = 0, carry = ( F & FLAG_C );
	if( ( F & FLAG_H ) || ( ( A & 0x0f ) > 9 ) ) add = 6;
	if( carry || ( A > 0x99 ) ) add |= 0x60;
	if( A > 0x99 ) carry = FLAG_C;
	if( F & FLAG_N ) {
	  SUB(add);
	} else {
	  ADD(A,add);
	}
	F = ( F & ~( FLAG_C | FLAG_P ) ) | carry |(4-__builtin_parity(A)*4);
      }
      //~ break;
	
	//~ DAA();
	T_WAIT_UNTIL(4);
break;
/*JR Z, %*/
case 0x28:
	d.u=NEXT_BYTE;
	if(F&FLAG_Z)
	{
		JR(d.s);
		T_WAIT_UNTIL(12);
	}
	else
	{
		T_WAIT_UNTIL(7);
	}
break;
/*ADD HL, HL*/
case 0x29:
	ADD16(HL, HL);
	T_WAIT_UNTIL(11);
break;
/*LD HL, (@)*/
case 0x2a:
	nn=NEXT_WORD;
	temp16=READ_WORD(nn);
	LD_RP_FROM_ADDR_MPTR_16(HL, temp16, nn);
	T_WAIT_UNTIL(16);
break;
/*DEC HL*/
case 0x2b:
	DEC16(HL);
	T_WAIT_UNTIL(6);
break;
/*INC L*/
case 0x2c:
	INC(L);
	T_WAIT_UNTIL(4);
break;
/*DEC L*/
case 0x2d:
	DEC(L);
	T_WAIT_UNTIL(4);
break;
/*LD L, #*/
case 0x2e:
	n=NEXT_BYTE;
	LD(L, n);
	T_WAIT_UNTIL(7);
break;
/*CPL*/
case 0x2f:
	CPL();
	T_WAIT_UNTIL(4);
break;
/*JR NC, %*/
case 0x30:
	d.u=NEXT_BYTE;
	if(!(F&FLAG_C))
	{
		JR(d.s);
		T_WAIT_UNTIL(12);
	}
	else
	{
		T_WAIT_UNTIL(7);
	}
break;
/*LD SP, @*/
case 0x31:
	nn=NEXT_WORD;
	LD16(SP, nn);
	T_WAIT_UNTIL(10);
break;
/*LD (@), A*/
case 0x32:
	nn=NEXT_WORD;
	LD_A_TO_ADDR_MPTR(temp8, A, nn);
	WRITE_BYTE(nn, temp8);
	T_WAIT_UNTIL(13);
break;
/*INC SP*/
case 0x33:
	INC16(SP);
	T_WAIT_UNTIL(6);
break;
/*INC (HL)*/
case 0x34:
	temp8=READ_BYTE(HL);
	INC(temp8);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(11);
break;
/*DEC (HL)*/
case 0x35:
	temp8=READ_BYTE(HL);
	DEC(temp8);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(11);
break;
/*LD (HL), #*/
case 0x36:
	n=NEXT_BYTE;
	LD(n, n);
	WRITE_BYTE(HL, n);
	T_WAIT_UNTIL(10);
break;
/*SCF*/
case 0x37:
	SCF();
	T_WAIT_UNTIL(4);
break;
/*JR C, %*/
case 0x38:
	d.u=NEXT_BYTE;
	if(F&FLAG_C)
	{
		JR(d.s);
		T_WAIT_UNTIL(12);
	}
	else
	{
		T_WAIT_UNTIL(7);
	}
break;
/*ADD HL, SP*/
case 0x39:
	ADD16(HL, SP);
	T_WAIT_UNTIL(11);
break;
/*LD A, (@)*/
case 0x3a:
	nn=NEXT_WORD;
	temp8=READ_BYTE(nn);
	LD_A_FROM_ADDR_MPTR(A, temp8, nn);
	T_WAIT_UNTIL(13);
break;
/*DEC SP*/
case 0x3b:
	DEC16(SP);
	T_WAIT_UNTIL(6);
break;
/*INC A*/
case 0x3c:
	INC(A);
	T_WAIT_UNTIL(4);
break;
/*DEC A*/
case 0x3d:
	DEC(A);
	T_WAIT_UNTIL(4);
break;
/*LD A, #*/
case 0x3e:
	n=NEXT_BYTE;
	LD(A, n);
	T_WAIT_UNTIL(7);
break;
/*CCF*/
case 0x3f:
	CCF();
	T_WAIT_UNTIL(4);
break;
/*LD B, B*/
case 0x47:
case 0x40 ... 0x45:
	LD(B, RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*LD B, C*/
case 0x41:
	LD(B, C);
	T_WAIT_UNTIL(4);
break;
/*LD B, D*/
case 0x42:
	LD(B, D);
	T_WAIT_UNTIL(4);
break;
/*LD B, E*/
case 0x43:
	LD(B, E);
	T_WAIT_UNTIL(4);
break;
/*LD B, H*/
case 0x44:
	LD(B, H);
	T_WAIT_UNTIL(4);
break;
/*LD B, L*/
case 0x45:
	LD(B, L);
	T_WAIT_UNTIL(4);
break;
#endif
/*LD B, (HL)*/
case 0x46:
	temp8=READ_BYTE(HL);
	LD(B, temp8);
	T_WAIT_UNTIL(7);
break;
/*LD B, A*/
	//~ LD(B, A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*LD C, B*/
case 0x4f:
case 0x48 ... 0x4d:
	LD(C, RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*LD C, C*/
case 0x49:
	LD(C, C);
	T_WAIT_UNTIL(4);
break;
/*LD C, D*/
case 0x4a:
	LD(C, D);
	T_WAIT_UNTIL(4);
break;
/*LD C, E*/
case 0x4b:
	LD(C, E);
	T_WAIT_UNTIL(4);
break;
/*LD C, H*/
case 0x4c:
	LD(C, H);
	T_WAIT_UNTIL(4);
break;
/*LD C, L*/
case 0x4d:
	LD(C, L);
	T_WAIT_UNTIL(4);
break;
#endif
/*LD C, (HL)*/
case 0x4e:
	temp8=READ_BYTE(HL);
	LD(C, temp8);
	T_WAIT_UNTIL(7);
break;
/*LD C, A*/
	//~ LD(C, A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*LD D, B*/
case 0x57:
case 0x50 ... 0x55:
	LD(D, RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*LD D, C*/
case 0x51:
	LD(D, C);
	T_WAIT_UNTIL(4);
break;
/*LD D, D*/
case 0x52:
	LD(D, D);
	T_WAIT_UNTIL(4);
break;
/*LD D, E*/
case 0x53:
	LD(D, E);
	T_WAIT_UNTIL(4);
break;
/*LD D, H*/
case 0x54:
	LD(D, H);
	T_WAIT_UNTIL(4);
break;
/*LD D, L*/
case 0x55:
	LD(D, L);
	T_WAIT_UNTIL(4);
break;
#endif
/*LD D, (HL)*/
case 0x56:
	temp8=READ_BYTE(HL);
	LD(D, temp8);
	T_WAIT_UNTIL(7);
break;
/*LD D, A*/
	//~ LD(D, A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*LD E, B*/
case 0x5f:
case 0x58 ... 0x5d:
	LD(E, RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*LD E, C*/
case 0x59:
	LD(E, C);
	T_WAIT_UNTIL(4);
break;
/*LD E, D*/
case 0x5a:
	LD(E, D);
	T_WAIT_UNTIL(4);
break;
/*LD E, E*/
case 0x5b:
	LD(E, E);
	T_WAIT_UNTIL(4);
break;
/*LD E, H*/
case 0x5c:
	LD(E, H);
	T_WAIT_UNTIL(4);
break;
/*LD E, L*/
case 0x5d:
	LD(E, L);
	T_WAIT_UNTIL(4);
break;
#endif
/*LD E, (HL)*/
case 0x5e:
	temp8=READ_BYTE(HL);
	LD(E, temp8);
	T_WAIT_UNTIL(7);
break;
/*LD E, A*/
	//~ LD(E, A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*LD H, B*/
case 0x67:
case 0x60 ...0x65:
	LD(H, RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*LD H, C*/
case 0x61:
	LD(H, C);
	T_WAIT_UNTIL(4);
break;
/*LD H, D*/
case 0x62:
	LD(H, D);
	T_WAIT_UNTIL(4);
break;
/*LD H, E*/
case 0x63:
	LD(H, E);
	T_WAIT_UNTIL(4);
break;
/*LD H, H*/
case 0x64:
	LD(H, H);
	T_WAIT_UNTIL(4);
break;
/*LD H, L*/
case 0x65:
	LD(H, L);
	T_WAIT_UNTIL(4);
break;
#endif
/*LD H, (HL)*/
case 0x66:
	temp8=READ_BYTE(HL);
	LD(H, temp8);
	T_WAIT_UNTIL(7);
break;
/*LD H, A*/
	//~ LD(H, A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*LD L, B*/
case 0x6f:
case 0x68 ... 0x6d:
	LD(L, RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*LD L, C*/
case 0x69:
	LD(L, C);
	T_WAIT_UNTIL(4);
break;
/*LD L, D*/
case 0x6a:
	LD(L, D);
	T_WAIT_UNTIL(4);
break;
/*LD L, E*/
case 0x6b:
	LD(L, E);
	T_WAIT_UNTIL(4);
break;
/*LD L, H*/
case 0x6c:
	LD(L, H);
	T_WAIT_UNTIL(4);
break;
/*LD L, L*/
case 0x6d:
	LD(L, L);
	T_WAIT_UNTIL(4);
break;
#endif
/*LD L, (HL)*/
case 0x6e:
	temp8=READ_BYTE(HL);
	LD(L, temp8);
	T_WAIT_UNTIL(7);
break;
/*LD L, A*/
	//~ LD(L, A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*LD (HL), B*/
case 0x77:
case 0x70 ...0x75:
	LD(temp8, RC(opcode&7));
	WRITE_BYTE((HL), temp8);
	T_WAIT_UNTIL(7);
break;
#if 0
/*LD (HL), C*/
case 0x71:
	LD(temp8, C);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(7);
break;
/*LD (HL), D*/
case 0x72:
	LD(temp8, D);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(7);
break;
/*LD (HL), E*/
case 0x73:
	LD(temp8, E);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(7);
break;
/*LD (HL), H*/
case 0x74:
	LD(temp8, H);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(7);
break;
/*LD (HL), L*/
case 0x75:
	LD(temp8, L);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(7);
break;
#endif
/*HALT*/
case 0x76:
	HALT();
	PC--;
	T_WAIT_UNTIL(4);
break;
/*LD (HL), A*/
	//~ LD(temp8, A);
	//~ WRITE_BYTE(HL, temp8);
	//~ T_WAIT_UNTIL(7);
//~ break;
/*LD A, B*/
case 0x7f:
case 0x78 ... 0x7d:
	LD(A, RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*LD A, C*/
case 0x79:
	LD(A, C);
	T_WAIT_UNTIL(4);
break;
/*LD A, D*/
case 0x7a:
	LD(A, D);
	T_WAIT_UNTIL(4);
break;
/*LD A, E*/
case 0x7b:
	LD(A, E);
	T_WAIT_UNTIL(4);
break;
/*LD A, H*/
case 0x7c:
	LD(A, H);
	T_WAIT_UNTIL(4);
break;
/*LD A, L*/
case 0x7d:
	LD(A, L);
	T_WAIT_UNTIL(4);
break;
#endif
/*LD A, (HL)*/
case 0x7e:
	temp8=READ_BYTE(HL);
	LD(A, temp8);
	T_WAIT_UNTIL(7);
break;
/*LD A, A*/
	//~ LD(A, A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*ADD A, B*/
case 0x87:
case 0x80 ... 0x85:
	ADD(A, RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*ADD A, C*/
case 0x81:
	ADD(A, C);
	T_WAIT_UNTIL(4);
break;
/*ADD A, D*/
case 0x82:
	ADD(A, D);
	T_WAIT_UNTIL(4);
break;
/*ADD A, E*/
case 0x83:
	ADD(A, E);
	T_WAIT_UNTIL(4);
break;
/*ADD A, H*/
case 0x84:
	ADD(A, H);
	T_WAIT_UNTIL(4);
break;
/*ADD A, L*/
case 0x85:
	ADD(A, L);
	T_WAIT_UNTIL(4);
break;
#endif
/*ADD A, (HL)*/
case 0x86:
	temp8=READ_BYTE(HL);
	ADD(A, temp8);
	T_WAIT_UNTIL(7);
break;
/*ADD A, A*/
	//~ ADD(A, A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*ADDC A, B*/
case 0x8f:
case 0x88 ... 0x8d:
	ADDC(A, RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*ADDC A, C*/
case 0x89:
	ADDC(A, C);
	T_WAIT_UNTIL(4);
break;
/*ADDC A, D*/
case 0x8a:
	ADDC(A, D);
	T_WAIT_UNTIL(4);
break;
/*ADDC A, E*/
case 0x8b:
	ADDC(A, E);
	T_WAIT_UNTIL(4);
break;
/*ADDC A, H*/
case 0x8c:
	ADDC(A, H);
	T_WAIT_UNTIL(4);
break;
/*ADDC A, L*/
case 0x8d:
	ADDC(A, L);
	T_WAIT_UNTIL(4);
break;
#endif
/*ADDC A, (HL)*/
case 0x8e:
	temp8=READ_BYTE(HL);
	ADDC(A, temp8);
	T_WAIT_UNTIL(7);
break;
/*ADDC A, A*/
	//~ ADDC(A, A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*SUB B*/
case 0x97:
case 0x90 ... 0x95:
	SUB(RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*SUB C*/
case 0x91:
	SUB(C);
	T_WAIT_UNTIL(4);
break;
/*SUB D*/
case 0x92:
	SUB(D);
	T_WAIT_UNTIL(4);
break;
/*SUB E*/
case 0x93:
	SUB(E);
	T_WAIT_UNTIL(4);
break;
/*SUB H*/
case 0x94:
	SUB(H);
	T_WAIT_UNTIL(4);
break;
/*SUB L*/
case 0x95:
	SUB(L);
	T_WAIT_UNTIL(4);
break;
#endif
/*SUB (HL)*/
case 0x96:
	temp8=READ_BYTE(HL);
	SUB(temp8);
	T_WAIT_UNTIL(7);
break;
/*SUB A*/
	//~ SUB(A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*SUBC A, B*/
case 0x9f:
case 0x98 ... 0x9d:
	SUBC(A, RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*SUBC A, C*/
case 0x99:
	SUBC(A, C);
	T_WAIT_UNTIL(4);
break;
/*SUBC A, D*/
case 0x9a:
	SUBC(A, D);
	T_WAIT_UNTIL(4);
break;
/*SUBC A, E*/
case 0x9b:
	SUBC(A, E);
	T_WAIT_UNTIL(4);
break;
/*SUBC A, H*/
case 0x9c:
	SUBC(A, H);
	T_WAIT_UNTIL(4);
break;
/*SUBC A, L*/
case 0x9d:
	SUBC(A, L);
	T_WAIT_UNTIL(4);
break;
#endif
/*SUBC A, (HL)*/
case 0x9e:
	temp8=READ_BYTE(HL);
	SUBC(A, temp8);
	T_WAIT_UNTIL(7);
break;
/*SUBC A, A*/
	//~ SUBC(A, A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*AND B*/
case 0xa7:
case 0xa0 ... 0xa5:
	AND(RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*AND C*/
case 0xa1:
	AND(C);
	T_WAIT_UNTIL(4);
break;
/*AND D*/
case 0xa2:
	AND(D);
	T_WAIT_UNTIL(4);
break;
/*AND E*/
case 0xa3:
	AND(E);
	T_WAIT_UNTIL(4);
break;
/*AND H*/
case 0xa4:
	AND(H);
	T_WAIT_UNTIL(4);
break;
/*AND L*/
case 0xa5:
	AND(L);
	T_WAIT_UNTIL(4);
break;
#endif
/*AND (HL)*/
case 0xa6:
	temp8=READ_BYTE(HL);
	AND(temp8);
	T_WAIT_UNTIL(7);
break;
/*AND A*/
	//~ AND(A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*XOR B*/
case 0xaf:
case 0xa8 ... 0xad:
	XOR(RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*XOR C*/
case 0xa9:
	XOR(C);
	T_WAIT_UNTIL(4);
break;
/*XOR D*/
case 0xaa:
	XOR(D);
	T_WAIT_UNTIL(4);
break;
/*XOR E*/
case 0xab:
	XOR(E);
	T_WAIT_UNTIL(4);
break;
/*XOR H*/
case 0xac:
	XOR(H);
	T_WAIT_UNTIL(4);
break;
/*XOR L*/
case 0xad:
	XOR(L);
	T_WAIT_UNTIL(4);
break;
#endif
/*XOR (HL)*/
case 0xae:
	temp8=READ_BYTE(HL);
	XOR(temp8);
	T_WAIT_UNTIL(7);
break;
/*XOR A*/
	//~ XOR(A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*OR B*/
case 0xb7:
case 0xb0 ... 0xb5:
	OR(RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*OR C*/
case 0xb1:
	OR(C);
	T_WAIT_UNTIL(4);
break;
/*OR D*/
case 0xb2:
	OR(D);
	T_WAIT_UNTIL(4);
break;
/*OR E*/
case 0xb3:
	OR(E);
	T_WAIT_UNTIL(4);
break;
/*OR H*/
case 0xb4:
	OR(H);
	T_WAIT_UNTIL(4);
break;
/*OR L*/
case 0xb5:
	OR(L);
	T_WAIT_UNTIL(4);
break;
#endif
/*OR (HL)*/
case 0xb6:
	temp8=READ_BYTE(HL);
	OR(temp8);
	T_WAIT_UNTIL(7);
break;
/*OR A*/
	//~ OR(A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*CP B*/
case 0xbf:
case 0xb8 ... 0xbd:
	CP(RC(opcode&7));
	T_WAIT_UNTIL(4);
break;
#if 0
/*CP C*/
case 0xb9:
	CP(C);
	T_WAIT_UNTIL(4);
break;
/*CP D*/
case 0xba:
	CP(D);
	T_WAIT_UNTIL(4);
break;
/*CP E*/
case 0xbb:
	CP(E);
	T_WAIT_UNTIL(4);
break;
/*CP H*/
case 0xbc:
	CP(H);
	T_WAIT_UNTIL(4);
break;
/*CP L*/
case 0xbd:
	CP(L);
	T_WAIT_UNTIL(4);
break;
#endif
/*CP (HL)*/
case 0xbe:
	temp8=READ_BYTE(HL);
	CP(temp8);
	T_WAIT_UNTIL(7);
break;
/*CP A*/
	//~ CP(A);
	//~ T_WAIT_UNTIL(4);
//~ break;
/*RET NZ*/
case 0xc0:
	if(!(F&FLAG_Z))
	{
		RET();
		T_WAIT_UNTIL(11);
	}
	else 
	{
		T_WAIT_UNTIL(5);
	}
break;
/*POP BC*/
case 0xc1:
	POP(BC);
	T_WAIT_UNTIL(10);
break;
/*JP NZ, @*/
case 0xc2:
	nn=NEXT_WORD;
	if(!(F&FLAG_Z))
	{
		JP(nn);
		T_WAIT_UNTIL(10);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
/*JP @*/
case 0xc3:
	nn=NEXT_WORD;
	JP(nn);
	T_WAIT_UNTIL(10);
break;
/*CALL NZ, @*/
case 0xc4:
	nn=NEXT_WORD;
	if(!(F&FLAG_Z))
	{
		CALL(nn);
		T_WAIT_UNTIL(17);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
/*PUSH BC*/
case 0xc5:
	PUSH(BC);
	T_WAIT_UNTIL(11);
break;
/*ADD A, #*/
case 0xc6:
	n=NEXT_BYTE;
	ADD(A, n);
	T_WAIT_UNTIL(7);
break;
/*RST 0x00*/
case 0xc7:
	RST(0x00);
	T_WAIT_UNTIL(11);
break;
/*RET Z*/
case 0xc8:
	if(F&FLAG_Z)
	{
		RET();
		T_WAIT_UNTIL(11);
	}
	else 
	{
		T_WAIT_UNTIL(5);
	}
break;
/*RET*/
case 0xc9:
	RET();
	T_WAIT_UNTIL(10);
break;
/*JP Z, @*/
case 0xca:
	nn=NEXT_WORD;
	if(F&FLAG_Z)
	{
		JP(nn);
		T_WAIT_UNTIL(10);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
case 0xcb:
	//~ prefix=0xCB;
		opcode=NEXT_BYTE;
		R++;
#include "opcode_cb.h"
		m_cycle+=4+4;

	//~ no_int=1;
break;
/*CALL Z, @*/
case 0xcc:
	nn=NEXT_WORD;
	if(F&FLAG_Z)
	{
		CALL(nn);
		T_WAIT_UNTIL(17);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
/*CALL @*/
case 0xcd:
	nn=NEXT_WORD;
	CALL(nn);
	T_WAIT_UNTIL(17);
break;
/*ADDC A, #*/
case 0xce:
	n=NEXT_BYTE;
	ADDC(A, n);
	T_WAIT_UNTIL(7);
break;
/*RST 0x08*/
case 0xcf:
	RST(0x08);
	T_WAIT_UNTIL(11);
break;
/*RET NC*/
case 0xd0:
	if(!(F&FLAG_C))
	{
		RET();
		T_WAIT_UNTIL(11);
	}
	else 
	{
		T_WAIT_UNTIL(5);
	}
break;
/*POP DE*/
case 0xd1:
	POP(DE);
	T_WAIT_UNTIL(10);
break;
/*JP NC, @*/
case 0xd2:
	nn=NEXT_WORD;
	if(!(F&FLAG_C))
	{
		JP(nn);
		T_WAIT_UNTIL(10);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
/*OUT (#), A*/
case 0xd3:
	n=NEXT_BYTE;
	temp16=(n+(A<<8));
	OUT_A(temp16, A);
	T_WAIT_UNTIL(11);
break;
/*CALL NC, @*/
case 0xd4:
	nn=NEXT_WORD;
	if(!(F&FLAG_C))
	{
		CALL(nn);
		T_WAIT_UNTIL(17);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
/*PUSH DE*/
case 0xd5:
	PUSH(DE);
	T_WAIT_UNTIL(11);
break;
/*SUB #*/
case 0xd6:
	n=NEXT_BYTE;
	SUB(n);
	T_WAIT_UNTIL(7);
break;
/*RST 0x10*/
case 0xd7:
	RST(0x10);
	T_WAIT_UNTIL(11);
break;
/*RET C*/
case 0xd8:
	if(F&FLAG_C)
	{
		RET();
		T_WAIT_UNTIL(11);
	}
	else 
	{
		T_WAIT_UNTIL(5);
	}
break;
/*EXX*/
case 0xd9:
	EXX();
	T_WAIT_UNTIL(4);
break;
/*JP C, @*/
case 0xda:
	nn=NEXT_WORD;
	if(F&FLAG_C)
	{
		JP(nn);
		T_WAIT_UNTIL(10);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
/*IN A,(#)*/
case 0xdb:
	n=NEXT_BYTE;
	temp16=(n+(A<<8));
	IN_A(A, temp16);
	T_WAIT_UNTIL(11);
break;
/*CALL C, @*/
case 0xdc:
	nn=NEXT_WORD; 
	if(F&FLAG_C)
	{
		CALL(nn);
		T_WAIT_UNTIL(17);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
case 0xfd:
case 0xdd:
{
	u16 *pntReg = (opcode==0xdd)?(&IX):(&IY);
	prefix = NEXT_BYTE;
	R++;
#include "opcode_dd.h"
	m_cycle+=8;
}
break;

/*SUBC A, #*/
case 0xde:
	n=NEXT_BYTE;
	SUBC(A, n);
	T_WAIT_UNTIL(7);
break;
/*RST 0x18*/
case 0xdf:
	RST(0x18);
	T_WAIT_UNTIL(11);
break;
/*RET PO*/
case 0xe0:
	if(!(F&FLAG_P))
	{
		RET();
		T_WAIT_UNTIL(11);
	}
	else 
	{
		T_WAIT_UNTIL(5);
	}
break;
/*POP HL*/
case 0xe1:
	POP(HL);
	T_WAIT_UNTIL(10);
break;
/*JP PO, @*/
case 0xe2:
	nn=NEXT_WORD;
	if(!(F&FLAG_P))
	{
		JP(nn);
		T_WAIT_UNTIL(10);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
/*EX (SP), HL*/
case 0xe3:
	temp16=READ_WORD(SP);
	EX_MPTR(temp16, HL);
	WRITE_WORD((SP), temp16);
	T_WAIT_UNTIL(19);
break;
/*CALL PO, @*/
case 0xe4:
	nn=NEXT_WORD;
	if(!(F&FLAG_P))
	{
		CALL(nn);
		T_WAIT_UNTIL(17);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
/*PUSH HL*/
case 0xe5:
	PUSH(HL);
	T_WAIT_UNTIL(11);
break;
/*AND #*/
case 0xe6:
	n=NEXT_BYTE;
	AND(n);
	T_WAIT_UNTIL(7);
break;
/*RST 0x20*/
case 0xe7:
	RST(0x20);
	T_WAIT_UNTIL(11);
break;
/*RET PE*/
case 0xe8:
	if(F&FLAG_P)
	{
		RET();
		T_WAIT_UNTIL(11);
	}
	else 
	{
		T_WAIT_UNTIL(5);
	}
break;
/*JP HL*/
case 0xe9:
	JP_NO_MPTR(HL);
	T_WAIT_UNTIL(4);
break;
/*JP PE, @*/
case 0xea:
	nn=NEXT_WORD;
	if(F&FLAG_P)
	{
		JP(nn);
		T_WAIT_UNTIL(10);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
/*EX DE, HL*/
case 0xeb:
	EX(DE, HL);
	T_WAIT_UNTIL(4);
break;
/*CALL PE, @*/
case 0xec:
	nn=NEXT_WORD;
	if(F&FLAG_P)
	{
		CALL(nn);
		T_WAIT_UNTIL(17);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
case 0xed:
	//~ prefix=0xED;
	opcode=NEXT_BYTE;
	R++;
#include "opcode_ed.h"
	m_cycle+=4;

	//~ no_int=1;
break;
/*XOR #*/
case 0xee:
	n=NEXT_BYTE;
	XOR(n);
	T_WAIT_UNTIL(7);
break;
/*RST 0x28*/
case 0xef:
	RST(0x28);
	T_WAIT_UNTIL(11);
break;
/*RET P*/
case 0xf0:
	if(!(F&FLAG_S))
	{
		RET();
		T_WAIT_UNTIL(11);
	}
	else 
	{
		T_WAIT_UNTIL(5);
	}
break;
/*POP AF*/
case 0xf1:
{
	u16 tt;
	POP(tt);
	F = ((u8*)&tt)[0];
	A = ((u8*)&tt)[1];
}
	T_WAIT_UNTIL(10);
break;
/*JP P, @*/
case 0xf2:
	nn=NEXT_WORD;
	if(!(F&FLAG_S))
	{
		JP(nn);
		T_WAIT_UNTIL(10);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
/*DI*/
case 0xf3:
	DI();
	T_WAIT_UNTIL(4);
break;
/*CALL P, @*/
case 0xf4:
	nn=NEXT_WORD;
	if(!(F&FLAG_S))
	{
		CALL(nn);
		T_WAIT_UNTIL(17);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
/*PUSH AF*/
case 0xf5:
{
	u16 tt;
	((u8*)&tt)[0] = F;
	((u8*)&tt)[1] = A;
	PUSH(tt);
}
	T_WAIT_UNTIL(11);
break;
/*OR #*/
case 0xf6:
	n=NEXT_BYTE;
	OR(n);
	T_WAIT_UNTIL(7);
break;
/*RST 0x30*/
case 0xf7:
	RST(0x30);
	T_WAIT_UNTIL(11);
break;
/*RET M*/
case 0xf8:
	if(F&FLAG_S)
	{
		RET();
		T_WAIT_UNTIL(11);
	}
	else 
	{
		T_WAIT_UNTIL(5);
	}
break;
/*LD SP, HL*/
case 0xf9:
	LD16(SP, HL);
	T_WAIT_UNTIL(6);
break;
/*JP M, @*/
case 0xfa:
	nn=NEXT_WORD;
	if(F&FLAG_S)
	{
		JP(nn);
		T_WAIT_UNTIL(10);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
/*EI*/
case 0xfb:
	EI();
	T_WAIT_UNTIL(4);
	interrupts_enabled_at = tstates+4;
break;
/*CALL M, @*/
case 0xfc:
	nn=NEXT_WORD;
	if(F&FLAG_S)
	{
		CALL(nn);
		T_WAIT_UNTIL(17);
	}
	else
	{
		MEMPTR=nn;
		T_WAIT_UNTIL(10);
	}
break;
/*CP #*/
case 0xfe:
	n=NEXT_BYTE;
	CP(n);
	T_WAIT_UNTIL(7);
break;
/*RST 0x38*/
case 0xff:
	RST(0x38);
	T_WAIT_UNTIL(11);
break;
}
}
