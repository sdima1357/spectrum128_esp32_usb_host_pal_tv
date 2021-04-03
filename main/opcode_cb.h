//~ void opcode_cb(u8 bt)
{
switch (opcode) 
{
/*RLC B*/
case 0x07:
case 0x00 ... 0x05:
	RLC(RC(opcode&7));
	EMPTY;
break;
/*RLC (HL)*/
case 0x06:
	temp8=READ_BYTE(HL);
	RLC(temp8);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(11-4);
break;

case 0x0f:
case 0x08 ... 0xd:
	RRC(RC(opcode&7));
	EMPTY;
break;
/*RRC (HL)*/
case 0x0e:
	temp8=READ_BYTE(HL);
	RRC(temp8);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(11-4);
break;
/*RL B*/
case 0x17:
case 0x10 ... 0x15:
	RL(RC(opcode&7));
	EMPTY;
break;
/*RL (HL)*/
case 0x16:
	temp8=READ_BYTE(HL);
	RL(temp8);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(11-4);
break;
/*RL A*/
//~ case 0x17:
	//~ RL(A);
	//~ EMPTY;
//~ break;
/*RR B*/
case 0x1f:
case 0x18 ... 0x1d:
	RR(RC(opcode&7));
	EMPTY;
break;
/*RR (HL)*/
case 0x1e:
	temp8=READ_BYTE(HL);
	RR(temp8);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(11-4);
break;
/*RR A*/
//~ case 0x1f:
	//~ RR(A);
	//~ EMPTY;
//~ break;
/*SLA B*/
case 0x27:
case 0x20 ... 0x25:
	SLA(RC(opcode&7));
	EMPTY;
break;
/*SLA (HL)*/
case 0x26:
	temp8=READ_BYTE(HL);
	SLA(temp8);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(11-4);
break;
/*SRA B*/
case 0x2f:
case 0x28 ... 0x2d:
	SRA(RC(opcode&7));
	EMPTY;
break;
case 0x2e:
	temp8=READ_BYTE(HL);
	SRA(temp8);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(11-4);
break;
case 0x37:
case 0x30 ... 0x35:
	SLL(RC(opcode&7));
	EMPTY;
break;

/*SLL (HL)*/
case 0x36:
	temp8=READ_BYTE(HL);
	SLL(temp8);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(11-4);
break;
/*SRL B*/
case 0x3f:
case 0x38 ... 0x3d:
	SRL(RC(opcode&7));
	EMPTY;
break;
/*SRL (HL)*/
case 0x3e:
	temp8=READ_BYTE(HL);
	SRL(temp8);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(11-4);
break;
/*BIT 0, B*/
case 0x47:
case 0x40 ... 0x45:
case 0x4f:
case 0x48 ... 0x4d:
case 0x57:
case 0x50 ... 0x55:
case 0x5f:
case 0x58 ... 0x5d:
case 0x67:
case 0x60 ... 0x65:
case 0x6f:
case 0x68 ... 0x6d:
case 0x77:
case 0x70 ... 0x75:
case 0x7f:
case 0x78 ... 0x7d:
	BIT(((opcode&0b111000)>>3), RC(opcode&7));
	EMPTY;
break;
/*BIT 0, (HL)*/
case 0x46:
case 0x4e:
case 0x56:
case 0x5e:
case 0x66:
case 0x6e:
case 0x76:
case 0x7e:
	temp8=READ_BYTE(HL);
	BIT_MPTR(((opcode&0b111000)>>3), temp8);
	T_WAIT_UNTIL(8-4);
break;

/*RES 0, B*/
case 0x87:
case 0x80 ... 0x85:
case 0x8f:
case 0x88 ... 0x8d:
case 0x97:
case 0x90 ... 0x95:
case 0x9f:
case 0x98 ... 0x9d:
case 0xa7:
case 0xa0 ... 0xa5:
case 0xaf:
case 0xa8 ... 0xad:
case 0xb7:
case 0xb0 ... 0xb5:
case 0xbf:
case 0xb8 ... 0xbd:
	RES(((opcode&0b111000)>>3), RC(opcode&7));
	EMPTY;
break;
case 0x86:
case 0x8e:
case 0x96:
case 0x9e:
case 0xa6:
case 0xae:
case 0xb6:
case 0xbe:
	temp8=READ_BYTE(HL);
	RES(((opcode&0b111000)>>3), temp8);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(11-4);
break;
/*SET 0, B*/
case 0xc7:
case 0xc0 ... 0xc5:
case 0xcf:
case 0xc8 ... 0xcd:

case 0xd7:
case 0xd0 ... 0xd5:
case 0xdf:
case 0xd8 ... 0xdd:

case 0xe7:
case 0xe0 ... 0xe5:
case 0xef:
case 0xe8 ... 0xed:

case 0xf7:
case 0xf0 ... 0xf5:
case 0xff:
case 0xf8 ... 0xfd:

	SET(((opcode&0b111000)>>3), RC(opcode&7));
	EMPTY;
break;
case 0xc6:
case 0xce:
case 0xd6:
case 0xde:
case 0xe6:
case 0xee:
case 0xf6:
case 0xfe:
	temp8=READ_BYTE(HL);
	SET(((opcode&0b111000)>>3), temp8);
	WRITE_BYTE(HL, temp8);
	T_WAIT_UNTIL(11-4);
break;
}
}
