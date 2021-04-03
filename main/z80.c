#include "z80.h"
//~ #include <stdio.h>
 
int32_t tstates      = 0;
int32_t interrupts_enabled_at;

/*include z80 registr*/
ZReg 		reg;
ZReg_Shadow 	reg_;
ZINTRFlags      IFlags;




#include "tables.h"

/*include opcodes*/


void z80_reset( int hard_reset )
{
	/*
		int i,j,k;
	uint8_t parity;
	
  for(i=0;i<0x100;i++) {
    sz53_table[i]= i & ( FLAG_3 | FLAG_5 | FLAG_S );
	if(i==0)
	{
		sz53_table[i] = 0x40;
	}
	//~ if(sz53_table[i]!=sz53_table_b[i])  
	//~ {
		//~ printf("sz53_table %x, %x, %x \n",i,(int)sz53_table[i],(int)sz53_table_b[i]);
	//~ }
    j=i; parity=0;
    for(k=0;k<8;k++) { parity ^= j & 1; j >>=1; }
    uint8_t parity_table_i = ( parity ? 0 : FLAG_P );
    sz53p_table[i] = sz53_table[i] | parity_table_i;
	//~ if(sz53p_table[i]!=sz53p_table_b[i])  
	//~ {
		//~ printf(" sz53p_table %x, %x, %x \n",i,(int)sz53p_table[i],(int)sz53p_table_b[i]);
	//~ }
  }

	*/
	
  AF_=0xffff;
  F=0xff;A=0xff;
  I=R=R7=0;
  PC=0;
  SP=0xffff;
  IFF1=IFF2=IM=0;
  halt=0;
  if( hard_reset ) {
    BC =DE =HL =0;
    BC_=DE_=HL_=0;
    IX=IY=0;
    MEMPTR=0;
  }
screen_IRQ = 0;
interrupts_enabled_at = 0;
tstates = 0;
	
	
  //~ z80.iff2_read=0;	
	//~ AF=0;
	//~ AF=0;
	//~ BC=0;
	//~ DE=0;
	//~ HL=0;
	//~ IX=0;
	//~ IY=0;
	//~ SP=0;
	//~ PC=0;
	//~ IFF1=IFF2=0;
	//~ IM=0;
	//~ MEMPTR=0;
  
	return;
}
//~ u16 i;
#define EMPTY ;
void z80_run(int num_reps)
{

union
{
	u8 u;
	s8 s;
}d;

u16 temp16;
u8   temp8;
u8   opcode;
u8   lookup;
u8   n;
u8   prefix;
u16 nn;
//~ u16 tempR;
int kk;
	//for(int k=0;k<num_reps;k++)
	for(;tstates<num_reps;)
	{
u16 m_cycle = 0;
		if (screen_IRQ&&IFF1&&tstates<32&&(tstates > interrupts_enabled_at))
		{
			if(halt)
			{
				PC++;
				halt = 0;
				tstates+=4;
			}
			//~ if(IFF1)
			{
				IFF1=0;
				IFF2=1;
				screen_IRQ=0;
				if(IM==1||IM==0)
				{
					R++;
					RST(0x38);
					tstates+=m_cycle;
				}
				else	if(IM==2)
				{
					uint16_t addr = (((uint16_t)(I))<<8)|0xff;
					uint16_t jmp_addr=peek16(addr);
					//~ printf("addr = %4x %4x\n",addr,jmp_addr);
					R++;
					RST(jmp_addr);
					tstates+=m_cycle;
				}
			}
			m_cycle = 0;
			//return;
		}
	opcode=NEXT_BYTE;
	R++;
#include "opcode_base.h"
	tstates+=m_cycle;
}
}
#if 0
void z80_run(void)
{

	union
{
	u8 u;
	s8 s;
}d;

u16 temp16;
u8   temp8;
u8   opcode;
u8   lookup;
u8   n;
u8   prefix;
u16 nn;
//~ u16 tempR;
int kk;

//~ CONT:	
	//~ if (halt==0)
//~ u8   prefix     = 0;	
	//~ const int rtable [] = {1,0,3,2,5,4,21,7};	//
#if 0
	if (screen_IRQ&&IFF1&&(tstates > interrupts_enabled_at))
	{
		if(halt)
		{
			PC++;
			halt = 0;
		}
		IFF1=0;
		IFF2=1;
		screen_IRQ=0;
		if(IM==1||IM==0)
		{
			RST(0x38);
			//~ tstates+=m_cycle;
		}
		else	if(IM==2)
		{
			uint16_t addr = (((uint16_t)I)<<8)|0xff;
			uint16_t jmp_addr=peek16(addr);
			//~ printf("addr = %4x %4x\n",addr,jmp_addr);
			RST(jmp_addr);
			//~ tstates+=m_cycle;
		}
		return;	
	} 
#endif
for(kk=0;kk<1024;kk++)
{
u16 m_cycle = 0;
	opcode=NEXT_BYTE;
	R++;
#include "opcode_base.h"
	tstates+=m_cycle;
}
}

#endif
