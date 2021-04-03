/*LD B, RLC (IX+$)*/
//~ void opcode_ddcb(u8 opcode)
{
	//~ if((bt>=0x40)&&(bt<0x80)) bt|=7; 
switch (prefix) 
{
case 0x00 ... 0x07:
	RLC(temp8);
	LD(RC(prefix&7), temp8);
	EMPTY;
	break;
/*LD B, RRC (IX+$)*/
case 0x08 ... 0x0f:
	RRC(temp8);
	LD(RC(prefix&7), temp8);
	EMPTY;
	break;
/*LD B, RL (IX+$)*/
case 0x10 ... 0x17:
	RL(temp8);
	LD16(RC(prefix&7), temp8);
	EMPTY;
	break;
/*LD B, RR (IX+$)*/
case 0x18 ... 0x1f:
	RR(temp8);
	LD16(RC(prefix&7), temp8);
	EMPTY;
	break;
/*LD B, SLA (IX+$)*/
case 0x20 ... 0x27:
	SLA(temp8);
	LD(RC(prefix&7), temp8);
	EMPTY;
	break;
/*LD B, SRA (IX+$)*/
case 0x28 ... 0x2f:
	SRA(temp8);
	LD(RC(prefix&7), temp8);
	EMPTY;
	break;
/*LD B, SLL (IX+$)*/
case 0x30 ... 0x37:
	SLL(temp8);
	LD(RC(prefix&7), temp8);
	EMPTY;
	break;
/*LD B, SRL (IX+$)*/
case 0x38 ... 0x3f:
	SRL(temp8);
	LD(RC(prefix&7), temp8);
	EMPTY;
	break;
/*BIT 0, (IX+$)*/
case 0x40 ... 0x7f:
	BIT_MPTR(((prefix&0b111000)>>3), temp8);
	T_WAIT_UNTIL(-3);
	break;
/*LD B, RES 0, (IX+$)*/
case 0x80 ... 0xbf:
	RES(((prefix&0b111000)>>3), temp8);
	LD(RC(prefix&7), temp8);
	EMPTY;
	break;
/*LD B, SET 0, (IX+$)*/
case 0xc0 ... 0xff:
	SET(((prefix&0b111000)>>3), temp8);
	LD(RC(prefix&7), temp8);
	EMPTY;
	break;
default:
	T_WAIT_UNTIL(-19);
}
}
