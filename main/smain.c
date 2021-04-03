#include "main.h"
#include "lcd.h"
#include "common_data.h"
int testTouch_dispatch(struct SYS_EVENT* ev){return 0;}
int clock_dispatch(struct SYS_EVENT* ev){return 0;}
int mp3_play_dispatch(struct SYS_EVENT* ev){return 0;}
union uCommonData CommonData;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define SPECTRUM_HEIGHT  192
#define SPECTRUM_WIDTH   256
//#define BORDER_WIDTH  (24)
//~ #define BORDER_HEIGHT  (16)
#define TAPE_LINES     (SH)
#define BORDER_HEIGHT ((SH-SPECTRUM_HEIGHT)/2)
#define BORDER_WIDTH ((SW-SPECTRUM_WIDTH)/2)
int  mp3_play(char* filename);
char* getFileName();
int   getModeFile();
void  DirtyMemorySet();
void tapeSetTime();
#include "z80.h"

#include "main_ROM.h"
#include "128-0.h"
#include "128-1.h"

const uint8_t  *  const  ROM_SINC[]=
{
	&fuse_roms_128_0_rom[0],
	&fuse_roms_128_1_rom[0],
	&fuse_roms_48_rom[0]
};


void tape_close();
int  tape_open(char* fileName);
void tapeStart();
void tapeStop();
int  getTapeState();
int  tapeBit();

extern int32_t tstates;
extern int32_t interrupts_enabled_at;
typedef struct
{
	unsigned int  color_curr:4;
	unsigned int  color_prev:4;
} borderArrayStruct;
struct sSINCLAIR_FLAGS
{
	int             flag128;
	uint16_t 	    border;
	borderArrayStruct 	borderArray[TAPE_LINES];
	int 			portMap   ;
	int 			ram_bank ;
	int 			rom_bank ;
	int 			shadow_bank;
	int 			T69888;
	int 			bColor;
	int     		joystickMode;
	uint8_t			IsPressed[8];
	uint8_t			Kempston;  //public
	u16 			prevKey;
	uint32_t        frameCounter;
	uint32_t        tape_frameCounter;
	int             tapeSpeed;
	int 			screenOffset;
} SINCLAIR_FLAGS;

void clearKeys()
{
	SINCLAIR_FLAGS.prevKey = 0xffff;
	SINCLAIR_FLAGS.IsPressed[0]= 0xff;
	SINCLAIR_FLAGS.IsPressed[1]= 0xff;
	SINCLAIR_FLAGS.IsPressed[2]= 0xff;
	SINCLAIR_FLAGS.IsPressed[3]= 0xff;
	SINCLAIR_FLAGS.IsPressed[4]= 0xff;
	SINCLAIR_FLAGS.IsPressed[5]= 0xff;
	SINCLAIR_FLAGS.IsPressed[6]= 0xff;
	SINCLAIR_FLAGS.IsPressed[7]= 0xff;
}
void initRamSpec()
{
	SINCLAIR_FLAGS.shadow_bank   = 5; // or 7
	SINCLAIR_FLAGS.rom_bank      = 0;
	SINCLAIR_FLAGS.ram_bank      = 0;
	SINCLAIR_FLAGS.portMap       = 0;
	SINCLAIR_FLAGS.T69888        = 70908;
	memset(SINCLAIR_FLAGS.borderArray,0xff,sizeof(SINCLAIR_FLAGS.borderArray));
	SINCLAIR_FLAGS.border = 0;
	SINCLAIR_FLAGS.frameCounter = 0;

	if(!SINCLAIR_FLAGS.flag128)
	{
		SINCLAIR_FLAGS.rom_bank = 2;
		SINCLAIR_FLAGS.portMap  = 0x20;
		SINCLAIR_FLAGS.T69888   = 69888;
	}
	else
	{

	}
	SINCLAIR_FLAGS.screenOffset = 1;
}

void init_Sinclair()
{
	SINCLAIR_FLAGS.bColor = 1;
	SINCLAIR_FLAGS.joystickMode = 0;
	SINCLAIR_FLAGS.Kempston = 0;
	SINCLAIR_FLAGS.prevKey = 0xffff;
	clearKeys();
	SINCLAIR_FLAGS.flag128 = 0;
	initRamSpec();
}
uint8_t getBorderColor(int line)
{
	return SINCLAIR_FLAGS.border;
}
int getT69888()
{
	return SINCLAIR_FLAGS.T69888;
}
uint32_t getFrameCounter()
{
	return SINCLAIR_FLAGS.frameCounter;
}


uint8_t    readByte(uint16_t adress);
void       writeByte(uint16_t adress,uint8_t data);

extern uint8_t * RAMCOMMON[];

#define  VIDEO_RAM (&RAMCOMMON[SINCLAIR_FLAGS.shadow_bank][0x0])
#define  ATTR_RAM  (&RAMCOMMON[SINCLAIR_FLAGS.shadow_bank][0x1800])

// declare for display
uint8_t* get_VIDEO_RAM()
{
	return VIDEO_RAM;
}
uint8_t* get_ATTR_RAM()
{
	return ATTR_RAM;
}

extern union uCommonData CommonData;

static const int levels[16] = {
  0x0000, 0x0385, 0x053D, 0x0770,
  0x0AD7, 0x0FD5, 0x15B0, 0x230C,
  0x2B4C, 0x43C1, 0x5A4B, 0x732F,
  0x9204, 0xAFF1, 0xD921, 0xFFFF
};
static int rng = 1;




uint16_t scanJSK();
void clearAttr()
{
#ifndef TV_OUT
	int k;
	for(k=0;k<((0x5B00-0x5800)>>3);k++)
	{
		CommonData.SincData.ATTR_RAM_MOD[k] = 0;
	}
#endif
}
void setAttr()
{
#ifndef TV_OUT
	int k;
	memset(SINCLAIR_FLAGS.borderArray,0xff,sizeof(SINCLAIR_FLAGS.borderArray));
	for(k=0;k<((0x5B00-0x5800)>>3);k++)
	{
		CommonData.SincData.ATTR_RAM_MOD[k] = 0xff;
	}
#endif
}
void setScreenDirty()
{
#ifndef TV_OUT
	int k;
	//memset(SINCLAIR_FLAGS.borderArray,0xff,sizeof(SINCLAIR_FLAGS.borderArray));
	for(k=0;k<((0x5B00-0x5800)>>3);k++)
	{
		CommonData.SincData.ATTR_RAM_MOD[k] = 0xff;
	}
#endif
}

void setMemPages(uint8_t val)
{
	if(SINCLAIR_FLAGS.portMap&0x20)
	{
		// locked
	}
	else
	{
		SINCLAIR_FLAGS.portMap   =  val;
		SINCLAIR_FLAGS.rom_bank 		=  (val&0x10)?1:0;
		int new_ram_bank 	=  val&0x7; 
		int new_shadow_bank = (val&0x8)?7:5;
		//if(new_shadow_bank!=SINCLAIR_FLAGS.shadow_bank|| ((new_ram_bank!=SINCLAIR_FLAGS.ram_bank)&&((new_ram_bank == 5) || (SINCLAIR_FLAGS.ram_bank == 5))))
		if(new_shadow_bank!=SINCLAIR_FLAGS.shadow_bank)
		{
			setScreenDirty();
		}
		SINCLAIR_FLAGS.shadow_bank = new_shadow_bank;
		SINCLAIR_FLAGS.ram_bank = new_ram_bank;
	}
}
u8  peek(uint16_t addr) 
{

	u8 res;
	//~ int page = addr>>14u;
	switch(addr>>14u)
	{
		case 0: 
		{
			//~ [(portMap>>5)&1]; 
			res = ROM_SINC[SINCLAIR_FLAGS.rom_bank][addr];
		}
		break;
		case 1:  
		{
				res = RAMCOMMON[5][addr-0x4000];
		}
		break;
		case 2:  	res = RAMCOMMON[2][addr-0x8000]; break;
		case 3:  
		default:
				res = RAMCOMMON[SINCLAIR_FLAGS.ram_bank][addr-0xc000]; break;
	};
	return res;
}
void  poke(uint16_t addr,uint8_t value) 
{
	switch(addr>>14u)
	{
		case 0: 
		{
			//~ [(portMap>>5)&1]; 
			//~ res = ROM[addr];  
			// write to rom??
		}
		break;
		case 1:  
		{
			tstates+=3;
			RAMCOMMON[5][addr-0x4000] = value;
			if(SINCLAIR_FLAGS.shadow_bank==5)
			{
				if(addr<0x5B00)
				{
#ifndef TV_OUT
					uint16_t pa = (addr<0x5800)?((addr&0x00ff) | (addr&0x1800)>>3):(addr-0x5800);
					CommonData.SincData.ATTR_RAM_MOD[pa>>3] |= (1<<(pa&0x3));
#endif
				}
			}
		}
		break;
		case 2:  RAMCOMMON[2][addr-0x8000] = value; break;
		case 3:  
			if(SINCLAIR_FLAGS.shadow_bank==7)
			{
				tstates+=3;
				if(addr<(0x5B00+0x8000))
				{
#ifndef TV_OUT
					uint16_t pa = (addr<(0x5800+0x8000))?((addr&0x00ff) | (addr&0x1800)>>3):(addr-(0x5800+0x8000));
					CommonData.SincData.ATTR_RAM_MOD[pa>>3] |= (1<<(pa&0x3));
#endif
				}
			}
			if (SINCLAIR_FLAGS.shadow_bank==5 && SINCLAIR_FLAGS.ram_bank==5)
			{
				poke(addr-0x8000,value);
			}
			else
			{
				RAMCOMMON[SINCLAIR_FLAGS.ram_bank][addr-0xc000] = value;
			}
			break;
	};
}

#if 0
//adddress bus:	//8	//9	//10	//11	//12	//13	//14	//15		
const int keyMatrix[5][8] = {
	{ CAPS_SHIFT	,'A'	,'Q'	,'1'	,'0'	,'P'	,ENTER	,SPACE		},	// d0
	{ 'Z'		,'S'	,'W'	,'2'	,'9'	,'O'	,'L'	,SYMB_SHIFT	},	// d1
	{ 'X'		,'D'	,'E'	,'3'	,'8'	,'I'	,'K'	,'M'		},	// d2
	{ 'C'		,'F'	,'R'	,'4'	,'7'	,'U'	,'J'	,'N'		},	// d3
	{ 'V'		,'G'	,'T'	,'5'	,'6'	,'Y'	,'H'	,'B'		}	// d4	
	};

const int myMatrix[3][4] = {
	{ 0x40		,' '	,0x10	,' '	},	// d
	{ ' '		,0x4	,' '	,0x8	},	// d
	{ 0x20		,' '	,0x1	,0x2	}	// d	
	};

#endif	

#define SPACE          	(((15-8)<<8)|0)
#define SYMB_SHIFT     	(((15-8)<<8)|1)
#define SKEY_M          	(((15-8)<<8)|2)
#define SKEY_N          	(((15-8)<<8)|3)
#define SKEY_B          	(((15-8)<<8)|4)

#define ENTER         	(((14-8)<<8)|0)
#define SKEY_L          	(((14-8)<<8)|1)
#define SKEY_K          	(((14-8)<<8)|2)
#define SKEY_J          	(((14-8)<<8)|3)
#define SKEY_H          	(((14-8)<<8)|4)

#define SKEY_P         	(((13-8)<<8)|0)
#define SKEY_O          	(((13-8)<<8)|1)
#define SKEY_I          	(((13-8)<<8)|2)
#define SKEY_U          	(((13-8)<<8)|3)
#define SKEY_Y          	(((13-8)<<8)|4)

#define SKEY_0         	(((12-8)<<8)|0)
#define SKEY_9          	(((12-8)<<8)|1)
#define SKEY_8          	(((12-8)<<8)|2)
#define SKEY_7          	(((12-8)<<8)|3)
#define SKEY_6          	(((12-8)<<8)|4)

#define SKEY_1         	(((11-8)<<8)|0)
#define SKEY_2          	(((11-8)<<8)|1)
#define SKEY_3          	(((11-8)<<8)|2)
#define SKEY_4          	(((11-8)<<8)|3)
#define SKEY_5          	(((11-8)<<8)|4)

#define SKEY_Q         	(((10-8)<<8)|0)
#define SKEY_W          	(((10-8)<<8)|1)
#define SKEY_E          	(((10-8)<<8)|2)
#define SKEY_R          	(((10-8)<<8)|3)
#define SKEY_T          	(((10-8)<<8)|4)

#define SKEY_A         	(((9-8)<<8)|0)
#define SKEY_S          	(((9-8)<<8)|1)
#define SKEY_D          	(((9-8)<<8)|2)
#define SKEY_F          	(((9-8)<<8)|3)
#define SKEY_G          	(((9-8)<<8)|4)

#define CAPS_SHIFT 		(((8-8)<<8) |0)
#define SKEY_Z          	(((8-8)<<8)|1)
#define SKEY_X          	(((8-8)<<8)|2)
#define SKEY_C          	(((8-8)<<8)|3)
#define SKEY_V          	(((8-8)<<8)|4)

inline void setRes(u8 adr,u8 bit,int setResFlag)
{
	if(setResFlag)
	{
		SINCLAIR_FLAGS.IsPressed[adr]|= 1u<<bit;
	}
	else
	{
		SINCLAIR_FLAGS.IsPressed[adr]&= ~(1u<<bit);
	}
		
}
void setResK(u16 key,int setResFlag)
{
	setRes(key>>8,key&0xff,setResFlag);
}

void SKEY_QUE_init()
{
	CommonData.SincData.SKEY_QUE_head = 0;
	CommonData.SincData.SKEY_QUE_tail = 0;
}
int32_t SKEY_QUE_size()
{
	return (CommonData.SincData.SKEY_QUE_head+SKEY_QUE_SIZE-CommonData.SincData.SKEY_QUE_tail)%SKEY_QUE_SIZE;
}

void SKEY_QUE_put(unsigned code,unsigned pressed,unsigned wait_for_next)
{
	struct SINCL_SKEY_EVENT ev;
	ev.code = code;
	ev.pressed = pressed;
	ev.wait_for_next = wait_for_next;
	if(SKEY_QUE_size()<SKEY_QUE_SIZE-1)
	{
		CommonData.SincData.SKEY_QUE[CommonData.SincData.SKEY_QUE_head] = ev;
		CommonData.SincData.SKEY_QUE_head = (CommonData.SincData.SKEY_QUE_head+1)% SKEY_QUE_SIZE;
	}
}
void SKEY_QUE_pop()
{
	if(SKEY_QUE_size())
	{
		CommonData.SincData.SKEY_QUE_tail = (CommonData.SincData.SKEY_QUE_tail+1)% SKEY_QUE_SIZE;
	}
}

char *joyMap[] =
{
		"76580 ",
		"98670~",
		"43125 ",
		"AZQW 0",
		"^v<>! "
};
char* JS_LINES[]=
{
		"  Cursor  ",
		"Sinclair 1",
		"Sinclair 2",
		"  AZQW    ",
		" Kempston "
};
#define joyMapSize (sizeof(joyMap)/sizeof(joyMap[0]))

extern MY_HID_KEYBD_Info_TypeDef prev_state_s;
MY_HID_KEYBD_Info_TypeDef prev_state_s_back;
#include <string.h>
int kkeyScanN;
u16 keyScan()
{
	u16 res = ~0;//~scanJSK();//0;//((KEYB_0_GPIO_Port->IDR>>3)&0b1111111);
	//if(res != SINCLAIR_FLAGS.prevKey)
	//kscan0();
	if(memcmp(&prev_state_s_back,&prev_state_s,sizeof(prev_state_s_back)))
	{
		kkeyScanN++;
		SINCLAIR_FLAGS.prevKey = res ;
		SINCLAIR_FLAGS.IsPressed[0] = 0xff;
		SINCLAIR_FLAGS.IsPressed[1] = 0xff;
		SINCLAIR_FLAGS.IsPressed[2] = 0xff;
		SINCLAIR_FLAGS.IsPressed[3] = 0xff;
		SINCLAIR_FLAGS.IsPressed[4] = 0xff;
		SINCLAIR_FLAGS.IsPressed[5] = 0xff;
		SINCLAIR_FLAGS.IsPressed[6] = 0xff;
		SINCLAIR_FLAGS.IsPressed[7] = 0xff;
		for(int k=0;k<4;k++)
		{
			switch(prev_state_s.keys[k])
			{
		    	case  0x4f: setResK(SKEY_8,0);setResK(CAPS_SHIFT,0);break;
			    case  0x50: setResK(SKEY_5,0);setResK(CAPS_SHIFT,0);break;
			    case  0x51: setResK(SKEY_6,0);setResK(CAPS_SHIFT,0);break;
				case  0x52: setResK(SKEY_7,0);setResK(CAPS_SHIFT,0);break;
				case  0x2a: setResK(SKEY_0,0);setResK(CAPS_SHIFT,0);break;

				case  0x1e: setResK(SKEY_1,0);break;
				case  0x1f: setResK(SKEY_2,0);break;
				case  0x20: setResK(SKEY_3,0);break;
				case  0x21: setResK(SKEY_4,0);break;
				case  0x22: setResK(SKEY_5,0);break;
				case  0x23: setResK(SKEY_6,0);break;
				case  0x24: setResK(SKEY_7,0);break;
				case  0x25: setResK(SKEY_8,0);break;
				case  0x26: setResK(SKEY_9,0);break;

				case  0x14: setResK(SKEY_Q,0);break;
				case  0x1a: setResK(SKEY_W,0);break;
				case  0x8:  setResK(SKEY_E,0);break;
				case  0x15: setResK(SKEY_R,0);break;
				case  0x17: setResK(SKEY_T,0);break;
				case  0x1c: setResK(SKEY_Y,0);break;
				case  0x18: setResK(SKEY_U,0);break;
				case  0xc:  setResK(SKEY_I,0);break;
				case  0x12: setResK(SKEY_O,0);break;
				case  0x13: setResK(SKEY_P,0);break;

				case  0x4:  setResK(SKEY_A,0);break;
				case  0x16: setResK(SKEY_S,0);break;
				case  0x7:  setResK(SKEY_D,0);break;
				case  0x9:  setResK(SKEY_F,0);break;
				case  0xa:  setResK(SKEY_G,0);break;
				case  0xb:  setResK(SKEY_H,0);break;
				case  0xd:  setResK(SKEY_J,0);break;
				case  0xe:  setResK(SKEY_K,0);break;
				case  0xf:  setResK(SKEY_L,0);break;


				case  0x1d: setResK(SKEY_Z,0);break;
				case  0x1b: setResK(SKEY_X,0);break;
				case  0x6:  setResK(SKEY_C,0);break;
				case  0x19: setResK(SKEY_V,0);break;
				case  0x5:  setResK(SKEY_B,0);break;
				case  0x11: setResK(SKEY_N,0);break;
				case  0x10: setResK(SKEY_M,0);break;


				case  0x27: setResK(SKEY_0,0);break;
				//case  0x0:  keyMatrix = 0;break;
				case  0x28: setResK(ENTER,0);break;//enterbreak;
				case  0x2c: setResK(SPACE,0);break;
				//case  0x29:  keyMatrix = K_ESC;break;
				//case  0x45:  keyMatrix = K_FIRE;break;
				//case  0x44:  keyMatrix = K_TOUCH;break;
			};
		}
		if(prev_state_s.lshift)
		{
			setResK(CAPS_SHIFT,0);
		}
		if(prev_state_s.rshift || prev_state_s.lctrl)
		{
			setResK(SYMB_SHIFT,0);
		}
		/*
		setResK(SKEY_7,res&K_UP);
		setResK(SKEY_6,res&K_DOWN);
		setResK(SKEY_5,res&K_LEFT);
		setResK(SKEY_8,res&K_RIGHT);

		setResK(SKEY_0,res&K_FIRE);

		setResK(SPACE,res&K_SPACE);  //space
		*/
		prev_state_s_back=prev_state_s;
	}
	return res^0b1111111;
#if 0
	/*
	uint8_t bt =0;
	if(check_char2(&bt))
	{
		if(bt==27)
		{
			if(check_char2(&bt))
			{
				if(bt==91)
				{
					if(check_char2(&bt))
					{
						switch(bt)
						{
							case 67 : // right
								res |=K_RIGHT; break;
							case 68 : // left
								res |=K_LEFT; break;
										//res&=~0x4; break;
							case 65 : // up
								res |=K_UP ;break;
										//res&=~0x10; break;
							case 66 : // down
								res |= K_DOWN; break;
										//res&=~0x1; break;
						}
					}
				}
			}
		}
		if(bt==13)
		{
			//res&=~0x20; // fire
			res|=K_FIRE;
		}
		if(bt==32)
		{
			res|=K_SPACE;
			//res&=~0x2; //space
		}
		if(bt==127)
		{
			//res&=~0x10; //fire
			res|=K_TOUCH;
		}
		else printf("got %d\n",bt);
	}
	*/
//	res = 0b1111111;
//	if(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)==GPIO_PIN_RESET)
//	{
//		res&=~0x20;
//	}
//	if(sample_touch())
//	{
//		res&=~2;
//		printf("space \r\n");
//	}

	if(res != SINCLAIR_FLAGS.prevKey)
	{
		SINCLAIR_FLAGS.prevKey = res ;
		SINCLAIR_FLAGS.Kempston = 0;
	if(SINCLAIR_FLAGS.joystickMode==0) //cursor
	{
//cursor joy matrix
//	up     - 7
//	down   - 6
//	left   - 5
//	right  - 8
// 	fire 	 0
//	special- " "
		setResK(SKEY_7,res&K_UP);
		setResK(SKEY_6,res&K_DOWN);
		setResK(SKEY_5,res&K_LEFT);
		setResK(SKEY_8,res&K_RIGHT);
		
		setResK(SKEY_0,res&K_FIRE);
		
		setResK(SPACE,res&K_SPACE);  //space
//		setRes(12-8,3,res&K_UP);
//		setRes(12-8,4,res&K_DOWN);
//		setRes(11-8,4,res&K_LEFT);
//		setRes(12-8,2,res&K_RIGHT);
//
//		setRes(12-8,0,res&K_FIRE);
//
//		setRes(15-8,0,res&K_TOUCH);  //space
	}
	else if((SINCLAIR_FLAGS.joystickMode)==1)
	{
//sinclair i1 joy matrix
//	up     - 9
//	down   - 8
//	left   - 6
//	right  - 7
// 	fire 	 0
		setResK(SKEY_9,res&K_UP);
		setResK(SKEY_8,res&K_DOWN);
		setResK(SKEY_6,res&K_LEFT);
		setResK(SKEY_7,res&K_RIGHT);
		
		setResK(SKEY_0,res&K_FIRE);
		
		setResK(ENTER,res&K_SPACE);  //space

//		setRes(12-8,1,res&K_UP);
//		setRes(12-8,2,res&K_DOWN);
//		setRes(12-8,4,res&K_LEFT);
//		setRes(12-8,3,res&K_RIGHT);
//
//		setRes(12-8,0,res&K_FIRE);
//
//		setRes(15-8,0,res&K_TOUCH);  //space
	}
	else if((SINCLAIR_FLAGS.joystickMode)==2)
	{
//sinclair j2 joy matrix
		setResK(SKEY_4,res&K_UP);
		setResK(SKEY_3,res&K_DOWN);
		setResK(SKEY_1,res&K_LEFT);
		setResK(SKEY_2,res&K_RIGHT);
		
		setResK(SKEY_5,res&K_FIRE);
		
		setResK(SPACE,res&K_SPACE);  //space

	}
	else if((SINCLAIR_FLAGS.joystickMode)==3)       //keyboard
	{
//sinclair key matrix
//	up     - a
//	down   - z
//	left   - q
//	right  - w
// 	fire 	 space
		setResK(SKEY_A,res&K_UP);
		setResK(SKEY_Z,res&K_DOWN);
		setResK(SKEY_Q,res&K_LEFT);
		setResK(SKEY_W,res&K_RIGHT);
		
		setResK(SPACE,res&K_FIRE);
		
		setResK(SPACE,res&K_SPACE);  //space

	}
	else if(SINCLAIR_FLAGS.joystickMode==4)
	{
// kempston joy on port 31
//	up     - 0x8
//	down   - 0x4
//	left   - 0x2
//	right  - 0x1
// 	fire 	 0x10
//
		SINCLAIR_FLAGS.Kempston = ~(((res&K_UP)?0x8:0)|((res&K_DOWN)?0x4:0)|((res&K_LEFT)?0x2:0)|((res&K_RIGHT)?0x1:0)|((res&K_FIRE)?0x10:0));
	}
	}
	return res^0b1111111;
#endif
}
int keyDelayCounter = 0;
uint16_t lastKeyEvent = 0;

uint8_t ay_read();

uint8_t lastByte;
u8 in(u16 port)
{

	u8 input=0xff;
	
	if ((port&0x00FF)==31)
	{
		keyScan();
		return SINCLAIR_FLAGS.Kempston;
	}
	if ((port&0x00FF)==0xFE)
	{
		if (keyDelayCounter>0) 
		{
			keyDelayCounter--;
		}
		if(keyDelayCounter==0)
		{
			if(SKEY_QUE_size())
			{
				struct SINCL_SKEY_EVENT ev = CommonData.SincData.SKEY_QUE[CommonData.SincData.SKEY_QUE_tail];
				setRes(ev.code>>8,ev.code&0xff,!ev.pressed);
				printf("ev.code %x ,ev.pressed %x ,ev.wait_for_next %x %d\r\n",ev.code,ev.pressed,ev.wait_for_next,SKEY_QUE_size());
				SKEY_QUE_pop();
				keyDelayCounter = ev.wait_for_next;
			}
			else
			{
				keyScan();
			}
		}

		int bit;
		for(bit = 0;bit<8; bit++)
		{
			if(!(port&(1<<(bit+8))))
			{
			   input &= SINCLAIR_FLAGS.IsPressed[bit];
			}
		}
		if(SINCLAIR_FLAGS.tapeSpeed!=0)
		{
			if(tapeBit()==0)
			{
				input&=0x10111111;
			}
		}
		tstates += 3;
	}
	else if(port==0x7ffd)
	{
		if(SINCLAIR_FLAGS.flag128)
			input = lastByte;
	}
	else if(port==0xfffd||port==0xfefd)
	{
		if(SINCLAIR_FLAGS.flag128)
		{
			input = ay_read();
		}
	}
	return input;

	

}

void aSOUND_EVENTS_init()
{
	CommonData.SincData.aSOUND_EVENTS_head = 0;
	CommonData.SincData.aSOUND_EVENTS_tail = 0;
}

inline int32_t aSOUND_EVENTS_size()
{
	return ((CommonData.SincData.aSOUND_EVENTS_head+SOUND_EVENT_ARRAY_SIZE)-CommonData.SincData.aSOUND_EVENTS_tail)%SOUND_EVENT_ARRAY_SIZE;
}

void aSOUND_EVENTS_put(union a_event ev)
{
	if(aSOUND_EVENTS_size()<SOUND_EVENT_ARRAY_SIZE-2)
	{
		CommonData.SincData.aSOUND_EVENTS[CommonData.SincData.aSOUND_EVENTS_head] = ev;
		CommonData.SincData.aSOUND_EVENTS_head = (CommonData.SincData.aSOUND_EVENTS_head+1)% SOUND_EVENT_ARRAY_SIZE;
	}
	else
	{

	}
}
void aSOUND_EVENTS_pop()
{
	if(aSOUND_EVENTS_size())
	{
		CommonData.SincData.aSOUND_EVENTS_tail = (CommonData.SincData.aSOUND_EVENTS_tail+1)% SOUND_EVENT_ARRAY_SIZE;
	}
}


volatile int soundBitState = 0;
//make  20msec sound buffer ~ 44100/50 = 882 samples
void initTick();

static const uint8_t ay_mask[ AY_REGISTERS ] = {

  0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0x1f, 0xff,
  0x1f, 0x1f, 0x1f, 0xff, 0xff, 0x0f, 0xff, 0xff,

};
void sound_ay_write(uint8_t current, uint8_t data,int32_t tstates_ay)
{
	union a_event r;

	r.u.t   = tstates_ay;
	r.u.ay =  1;
	r.u.data = data;
	r.u.cmd = current;
	//~ if(flag128)
	{

		aSOUND_EVENTS_put(r);

	}
}

void ay_reset()
{
	ayinfo *ay = &CommonData.SincData.ay_z80;
	ay->current_register = 0;
	memset( ay->registers, 0, sizeof( ay->registers ) );
	initTick();
}
uint8_t ay_read()
{
	int current;
	const uint8_t port_input = 0xbf;
	current = CommonData.SincData.ay_z80.current_register;
  if( current == 14 ) {
    if(CommonData.SincData.ay_z80.registers[7] & 0x40)
      return (port_input & CommonData.SincData.ay_z80.registers[14]);
    else
      return port_input;
    }
/* R15 is simpler to do, as the 8912 lacks the second I/O port, and
     the input-mode input is always 0xff */
  if( current == 15 && !( CommonData.SincData.ay_z80.registers[7] & 0x80 ) )
    return 0xff;

  /* Otherwise return register value, appropriately masked */
  return CommonData.SincData.ay_z80.registers[ current ] & ay_mask[ current ];


}
void ay_write_command(uint8_t cmd)
{
	CommonData.SincData.ay_z80.current_register = (cmd & 0x0f);
}
void ay_write_data(uint8_t data)
{
  int current;

  current = CommonData.SincData.ay_z80.current_register;

  CommonData.SincData.ay_z80.registers[ current ] = data & ay_mask[ current ];

  sound_ay_write( current, data, tstates );
  //~ if( psg_recording ) psg_write_register( current, b );

  if( current == 14 )
  {
	 // printer_serial_write( b );!!!
  }
}
#define ATTACK_BIT 		0b0100
#define ALTERNATE_BIT 	0b0010
#define HOLD_BIT 		0b0001
#define CONTINUE_BIT	0b1000

void incPattern()
{
	//~ int flgEnd = count>15;
	if(CommonData.SincData.soundModule_sm.pattern_nu&CONTINUE_BIT)
	{
		CommonData.SincData.soundModule_sm.val = CommonData.SincData.soundModule_sm.count&0xf;
		if(((CommonData.SincData.soundModule_sm.pattern_nu&ALTERNATE_BIT)&&(CommonData.SincData.soundModule_sm.count&0x10))^(!(CommonData.SincData.soundModule_sm.pattern_nu&ATTACK_BIT)))
		{
			CommonData.SincData.soundModule_sm.val = 0xf-CommonData.SincData.soundModule_sm.val;
		}
		if((CommonData.SincData.soundModule_sm.pattern_nu&HOLD_BIT)&&(CommonData.SincData.soundModule_sm.count>0xf))
		{
			CommonData.SincData.soundModule_sm.val = ((CommonData.SincData.soundModule_sm.pattern_nu^(CommonData.SincData.soundModule_sm.pattern_nu<<1))&ATTACK_BIT)?0xf:0;
		}
	}
	else
	{
		CommonData.SincData.soundModule_sm.val = (CommonData.SincData.soundModule_sm.count>0xf)?0:((CommonData.SincData.soundModule_sm.pattern_nu&ATTACK_BIT)?CommonData.SincData.soundModule_sm.count:(0xf-CommonData.SincData.soundModule_sm.count));
	}
	CommonData.SincData.soundModule_sm.count++;
}
int initPattern(int _pattern_nu)
{
	CommonData.SincData.soundModule_sm.count = 0;
	CommonData.SincData.soundModule_sm.pattern_nu = _pattern_nu;
	incPattern();
	return 0;
}

void initTick()
{
	for(int k=0;k<NUM_TONES;k++)
	{
		CommonData.SincData.soundModule_sm.counters[k] = 0;
		CommonData.SincData.soundModule_sm.counters_period[k] = 0;
		CommonData.SincData.soundModule_sm.counters_state[k] =1;
		CommonData.SincData.soundModule_sm.tick_inv[k ] =0;
	}
	for(int k=0;k<16;k++)
	{
		CommonData.SincData.soundModule_sm.reg[k ] =0;
	}
}
void clearTick()
{
 for(int k=0;k<NUM_TONES;k++)
 {
	 CommonData.SincData.soundModule_sm.tick_inv[k ] = 0;
 }

}
void incrTick(int tiks)
{
	//printf("tiks = %d \n",tiks);

	//kscan0();
	for(int k=0;k<NUM_TONES;k++)
	{
		CommonData.SincData.soundModule_sm.counters[k]+= tiks;
		int divider = CommonData.SincData.soundModule_sm.counters_period[k];
		int mtp = CommonData.SincData.soundModule_sm.counters_period_inv[k];
		//if(divider==0)
		{
			//divider=1;
		}
		//int cl = CommonData.SincData.soundModule_sm.counters[k]/divider;
		int cl = CommonData.SincData.soundModule_sm.counters[k]*mtp>>16;
		if(cl&0x1)
		{
			CommonData.SincData.soundModule_sm.counters_state[k] =  -CommonData.SincData.soundModule_sm.counters_state[k];
		}
		CommonData.SincData.soundModule_sm.counters[k]-=divider*cl;
		cl = cl&0x1f;
		if(k==NUM_TONES-2)
		{
			for(int j=0;j<cl;j++)
			{
				if( ( rng & 1 ) ^ ( ( rng & 2 ) ? 1 : 0 ) )
					CommonData.SincData.soundModule_sm.noise_toggle = !CommonData.SincData.soundModule_sm.noise_toggle;
				  if( rng & 1 )
				  {
					  rng ^= 0x24000;
				  }
				  rng >>= 1;
			}
		}
		if(k==NUM_TONES-1)
		{
			for(int j=0;j<cl;j++)
			{
				incPattern();
			}
		}
	}
//	for(int k=0;k<NUM_TONES;k++)
//	{
//		soundModule_sm.counters[k]+= tiks;
//		int divider = soundModule_sm.counters_period[k];
//		while(soundModule_sm.counters[k]>=divider)
//		{
//			soundModule_sm.counters[k]-=divider;
//			soundModule_sm.counters_state[k] =  -soundModule_sm.counters_state[k];
//			if(k==NUM_TONES-2)
//			{
//				if( ( rng & 1 ) ^ ( ( rng & 2 ) ? 1 : 0 ) )
//				noise_toggle = !noise_toggle;
//				  if( rng & 1 ) {
//				rng ^= 0x24000;
//				  }
//				  rng >>= 1;
//			}
//			if(k==NUM_TONES-1)
//			{
//				incPattern();
//			}
//			soundModule_sm.tick_inv[k]++;
//		}
//	}
	//~ int k =

}
//~ int volume[3];
inline int getVolPattern()
{
	return levels[CommonData.SincData.soundModule_sm.val&0xf]/8;
}

inline int getVolume(int chan)
{
	//~ PRINT(int(reg[8+chan]));
	return (CommonData.SincData.soundModule_sm.reg[8+chan]&0x10)?getVolPattern():((levels[CommonData.SincData.soundModule_sm.reg[8+chan]&0xf])/8);
}
inline int noiseBit()
{
	return (CommonData.SincData.soundModule_sm.noise_toggle&1)*2-1;
}
inline int getSign()
{
	int amp = 0;
	if(!SINCLAIR_FLAGS.flag128)
	{
		return amp;
	}
	//~ PRINT(amp);
	if((CommonData.SincData.soundModule_sm.reg[7]&1)==0)
	{
		amp+= getVolume(0)*CommonData.SincData.soundModule_sm.counters_state[0];
	}
	//~ PRINT(amp);
	if((CommonData.SincData.soundModule_sm.reg[7]&2)==0)
	{
		amp+= getVolume(1)*CommonData.SincData.soundModule_sm.counters_state[1];
	}
	//~ PRINT(amp);
	if((CommonData.SincData.soundModule_sm.reg[7]&4)==0)
	{
		amp+= getVolume(2)*CommonData.SincData.soundModule_sm.counters_state[2];
	}
	//~ PRINT(amp);
	if((CommonData.SincData.soundModule_sm.reg[7]&8)==0)
	{
		amp+= getVolume(0)/2*noiseBit();
	}
	if((CommonData.SincData.soundModule_sm.reg[7]&0x10)==0)
	{
		amp+= getVolume(1)/2*noiseBit();
	}
	if((CommonData.SincData.soundModule_sm.reg[7]&0x20)==0)
	{
		amp+= getVolume(2)/2*noiseBit();
	}

	return amp;
}
#define RSCALE (16)
void execute(union a_event* ev)
{
	if(!ev->u.ay) return;
	CommonData.SincData.soundModule_sm.reg[ev->u.cmd&0xf] = ev->u.data;
	//cout<<hex;
	switch(ev->u.cmd)
	{
		case 0:
		case 1:  CommonData.SincData.soundModule_sm.counters_period[0] = ( (256*(int)(CommonData.SincData.soundModule_sm.reg[1])) +((int)(CommonData.SincData.soundModule_sm.reg[0])))*RSCALE ;
					CommonData.SincData.soundModule_sm.counters_period_inv[0] = CommonData.SincData.soundModule_sm.counters_period[0]?65536/CommonData.SincData.soundModule_sm.counters_period[0]:65536;
					break;
		case 2:
		case 3:  CommonData.SincData.soundModule_sm.counters_period[1] = ((256*(int)(CommonData.SincData.soundModule_sm.reg[3])) +((int)(CommonData.SincData.soundModule_sm.reg[2])))*RSCALE ;
					CommonData.SincData.soundModule_sm.counters_period_inv[1] = CommonData.SincData.soundModule_sm.counters_period[1]?65536/CommonData.SincData.soundModule_sm.counters_period[1]:65536;
					break;
		case 4:
		case 5:  CommonData.SincData.soundModule_sm.counters_period[2] = ((256*(int)(CommonData.SincData.soundModule_sm.reg[5])) +((int)(CommonData.SincData.soundModule_sm.reg[4])))*RSCALE ;
					CommonData.SincData.soundModule_sm.counters_period_inv[2] = CommonData.SincData.soundModule_sm.counters_period[2]?65536/CommonData.SincData.soundModule_sm.counters_period[2]:65536;
					break;
		case 6:  CommonData.SincData.soundModule_sm.counters_period[3] = (int)(CommonData.SincData.soundModule_sm.reg[6])*RSCALE ;
					CommonData.SincData.soundModule_sm.counters_period_inv[3] = CommonData.SincData.soundModule_sm.counters_period[3]?65536/CommonData.SincData.soundModule_sm.counters_period[3]:65536;
					break;
		case 7:   break;
		case 8:   break;
		case 9:   break;
		case 10: break;
		case 11:
		case 12: CommonData.SincData.soundModule_sm.counters_period[4] = ((256*(int)(CommonData.SincData.soundModule_sm.reg[12])) +((int)(CommonData.SincData.soundModule_sm.reg[11])))*RSCALE;
					CommonData.SincData.soundModule_sm.counters_period_inv[4] = CommonData.SincData.soundModule_sm.counters_period[4]?65536/CommonData.SincData.soundModule_sm.counters_period[4]:65536;
					break;
		case 13: initPattern(CommonData.SincData.soundModule_sm.reg[13]); break;
		case 14: break;
		case 15: break;
	};
}


void soundEvents(int event)
{
	union a_event r;
	r.u.t    = tstates;
	r.u.ay   = 0;
	r.u.data = event;
	r.u.cmd  = 0;
	aSOUND_EVENTS_put(r);
}

//static int32_t fuller_next = 0;
static int32_t fuller = 0;
void push_pair(uint16_t,uint16_t);
int waitFor();

#include <math.h>
int  sound_size();
void procKeyb()
{
	//~ static uint32_t prev_t ;
	//~ uint32_t last = HAL_GetTick();
	//if(last!=prev_t)
	{
		//~ prev_t = last;
		kscan0();
	}
}
void proccesSoundEvents_time()
{
	// wait interrupt for free buffer
	//  get last time schedule event;
//	int cntEvents = 0;
	soundEvents(100);

	union a_event next    = CommonData.SincData.aSOUND_EVENTS[CommonData.SincData.aSOUND_EVENTS_tail];

	int32_t currentTime  =  next.u.t;
	int32_t sheduledTime =  currentTime;
#define TIME_CONST (15625/50)	
        //-
	int tclockPerSoundRate = (SINCLAIR_FLAGS.T69888 / (TIME_CONST));
	if(getTapeState()&&(SINCLAIR_FLAGS.tapeSpeed==2))
	{
		tclockPerSoundRate*=5;
	}
   // int old_size = aSOUND_EVENTS_size();
   // int sound_size1 = sound_size();
   // int rdelay = waitFor();
	//waitFor();
    //int cntPush = 0;
    //printf("flag128=%d\r\n",flag128);
    while(aSOUND_EVENTS_size())
    {
    	int amp = 0;

    	if(currentTime>=sheduledTime)
    	{
    		if(SINCLAIR_FLAGS.flag128)
    		{
    			execute(&next);
    		}

			if((next.u.ay==0)&&((next.u.data==0)||(next.u.data==1)))
			{
				//fuller = fuller_next;
				fuller = next.u.data?(MAX_VOLUME/16):(-MAX_VOLUME/16);
				//fuller_next = next.u.data?(MAX_VOLUME/64):(-MAX_VOLUME/64);
			}
			next  = CommonData.SincData.aSOUND_EVENTS[CommonData.SincData.aSOUND_EVENTS_tail];

			sheduledTime = next.u.t;
			aSOUND_EVENTS_pop();
    	}
    	else
    	{
			if(SINCLAIR_FLAGS.flag128)
			{
				//soundModule_sm.
				incrTick(tclockPerSoundRate);
				amp = (getSign())*MAX_VOLUME/0x10000;
			}
			//cntPush++;
			//waitFor();
			//fuller =fuller*990/1024;
			push_pair(fuller+amp+MAX_VOLUME/2,fuller+amp+MAX_VOLUME/2);
	//		cntEvents++;

			currentTime += tclockPerSoundRate;
    		procKeyb();
    	}
    }
    while(currentTime<sheduledTime)
    {
    	int amp = 0;
    	if(SINCLAIR_FLAGS.flag128)
    	{
    		//soundModule_sm.

			incrTick(tclockPerSoundRate);
			amp = (getSign())*MAX_VOLUME/0x10000;
			//amp = (getSign()+0x8000)*1903/0x10000;
    	}
    	//cntPush++;
    	//waitFor();
    		//fuller =fuller*990/1024;
		push_pair(fuller+amp+MAX_VOLUME/2,fuller+amp+MAX_VOLUME/2);
		//cntEvents++;
    	currentTime += tclockPerSoundRate;
		procKeyb();
    }
	if(SINCLAIR_FLAGS.flag128)
	{
		execute(&next);
	}
	if((next.u.ay==0)&&((next.u.data==0)||(next.u.data==1)))
	{
		//fuller = fuller_next;
		fuller = next.u.data?(MAX_VOLUME/16):(-MAX_VOLUME/16);
		//fuller_next = next.u.data?(MAX_VOLUME/64):(-MAX_VOLUME/64);
	}
/*
	static int cntt = 0;
	cntt++;
	if((cntt&0x1f)==0)
	{
		printf("numEvevnts = %d\n",cntEvents);
	}
*/
    //int sound_size2 = sound_size();
   // printf("sound events_size %d delay %d cntPush = %d %d %d\r\n",old_size,rdelay,cntPush,sound_size1,sound_size2);
}
//1000 8
//1001 9
//1010 A
//1011 B
//1100 C
//1101 D
//1110 E
//1111 F

void out(u16 port, u8 value)
{
	if ((port&0xFF)==0x00FE)
	{
		//if(SINCLAIR_FLAGS.border != (value&0x7)||tstates <1000)
		{
			//625/2
			SINCLAIR_FLAGS.border = value&0x7;
#warning "border history!!"
#if 1
			{
				int indB = (TAPE_LINES+20)*(tstates)/SINCLAIR_FLAGS.T69888-10;
				if(indB<0)
				{
					indB = 0;
				}
				if(indB>TAPE_LINES-1)indB = TAPE_LINES-1;
				SINCLAIR_FLAGS.borderArray[indB].color_curr = SINCLAIR_FLAGS.border;
				//printf("out[%04x]=%02x %x %x %x\n",port,value,tstates,tstates- prev_t,indB);
			}
#endif
		}
		if(value&(0b10000)) //sound bit
		{
			if(soundBitState!=1)
			{
				soundBitState = 1;
				soundEvents(1);
			}
		}
		else
		{
			if(soundBitState!=0)
			{
				soundBitState = 0;
				soundEvents(0);
			}
		}
		//1000 8
		//1001 9
		//1010 a
		//1011 b
		//1100 c
		//1101 d
		//1110 e

	}
	else if((port&0x8002)==0)
	{
		lastByte  = value;
		setMemPages(value);
	}
	else if(port==0xfffd||port==0xfefd)
	{
		if(SINCLAIR_FLAGS.flag128)
		{
			ay_write_command(value);
		}
	}
	else if(port==0xbffd||port==0xbefd)
	{
		if(SINCLAIR_FLAGS.flag128)
		{
			ay_write_data(value);
		}
	}
}
void poke16(u16 addr, u16 value)
{
	poke(addr, value);
	poke(addr+1, value>>8);
}

u16 peek16(u16 addr)
{
	return ((peek(addr+1)<<8)|peek(addr));
}

void SB_init()
{
	CommonData.SincData.SB.head = 0;
	CommonData.SincData.SB.tail    = 0;
}
void SB_put(u8 bt)
{
	CommonData.SincData.SB.bb[CommonData.SincData.SB.head] = bt;
	CommonData.SincData.SB.head++;
	CommonData.SincData.SB.head&=0x3f;
}
u8 SB_get()
{
	u8 res = CommonData.SincData.SB.bb[CommonData.SincData.SB.tail];
	CommonData.SincData.SB.tail++;
	CommonData.SincData.SB.tail&=0x3f;
	return res;
}
int SB_size()
{
	return (CommonData.SincData.SB.head-CommonData.SincData.SB.tail)&0x3f;
}
int f_read_b(FILE** fp, void* buff, uint32_t btr, uint32_t* br)
{
	int k;
	if(SB_size()<btr)
	{
		uint32_t bytes = 0;
		u8 bbt[0x20];
		//f_read(fp,bbt,0x20,&bytes);
		bytes = fread(bbt,1,0x20,*fp);
		for(k=0;k<bytes;k++)
		{
			SB_put(bbt[k]);
		}
	}
        u8 *pnt =buff;
	int rb = 0;
	for(k=0;k<btr&&SB_size();k++)
	{
		pnt[k] = SB_get();
		rb++;
	}
	*br = rb;
	return 0;
}
void poke_page(int page,u16 addr, u8 value)
{
	RAMCOMMON[page][addr&0x3fff] = value;
}
extern FILE* ifs_tape;
#define MyFile (ifs_tape)
int fwriteFile_vers(char* fileName)
{
	//FIL 		MyFile;
	MyFile = fopen(fileName,"wb");
	uint8_t bu8=0;
	uint16_t bu16=0;
	uint32_t BytesWrite;
	bu8 = A; fwrite(&bu8,1,1,MyFile) ;
	bu8 = F; fwrite(&bu8,1,1,MyFile) ;
	fwrite(&BC,1,2,MyFile);
	fwrite(&HL,1,2,MyFile);
	bu16 = 0; //version = 2;
	fwrite(&bu16,1,2,MyFile);
	fclose(MyFile);
	MyFile = NULL;
	
	
#if 0	
	if(f_open(&MyFile, fileName, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	{
		return 0;
	}

	uint8_t bu8=0;
	uint16_t bu16=0;
	UINT BytesWrite;
	bu8 = A; f_write(&MyFile,&bu8,1,&BytesWrite); ;
	bu8 = F; f_write(&MyFile,&bu8,1,&BytesWrite); ;
	f_write(&MyFile,&BC,2,&BytesWrite);
	f_write(&MyFile,&HL,2,&BytesWrite);
	bu16 = 0; //version = 2;

	f_write(&MyFile,&bu16,2,&BytesWrite);
	f_close(&MyFile);
#endif	
	return 1;
}

int freadFile_vers(char* fileName)
{
	//FIL 		MyFile;
	ifs_tape = fopen(fileName,"rb");
	//if(f_open(&MyFile,(TCHAR*)fileName, FA_READ)==FR_OK)
	if(!ifs_tape)
	{
		printf("open %s ERR\n",fileName);	
	}
	if(ifs_tape)
	{
	printf("open %s OK\n",fileName);	
	clearFullScreen();
	SB_init();
	uint8_t bu8=0;
	uint16_t bu16=0;
	uint32_t BytesRead;

	if(strcasestr(fileName,".sna"))
	{
		printf("read SNA!!!\n\r");
		MEMPTR = 0;
		tstates = 11200;
		interrupts_enabled_at = 0;

		SINCLAIR_FLAGS.flag128 = 0;
		initRamSpec();
		f_read_b(&MyFile,&bu8,1,&BytesRead); I = bu8;
		f_read_b(&MyFile,&HL_,2,&BytesRead);
		f_read_b(&MyFile,&DE_,2,&BytesRead);
		f_read_b(&MyFile,&BC_,2,&BytesRead);
		f_read_b(&MyFile,&bu8,1,&BytesRead); A_ = bu8;
		f_read_b(&MyFile,&bu8,1,&BytesRead); F_ = bu8;

		f_read_b(&MyFile,&HL,2,&BytesRead);
		f_read_b(&MyFile,&DE,2,&BytesRead);
		f_read_b(&MyFile,&BC,2,&BytesRead);
		f_read_b(&MyFile,&IY,2,&BytesRead);
		f_read_b(&MyFile,&IX,2,&BytesRead);

		f_read_b(&MyFile,&bu8,1,&BytesRead); IFF2 = (bu8&4)?1:0;

		f_read_b(&MyFile,&bu8,1,&BytesRead); R = bu8;

		f_read_b(&MyFile,&bu8,1,&BytesRead); A = bu8;
		f_read_b(&MyFile,&bu8,1,&BytesRead); F = bu8;

		f_read_b(&MyFile,&SP,2,&BytesRead);

		f_read_b(&MyFile,&bu8,1,&BytesRead); IM   = bu8&3;
		f_read_b(&MyFile,&bu8,1,&BytesRead);  SINCLAIR_FLAGS.border = bu8;
		int addr = 0x4000;
		//~ uint8_t rl;
		{
			//read 48K raw
			for(int k=0;k<0x10000-0x4000;k++)
			{
				//ifs.read((char*)&bu8,1);
				f_read_b(&MyFile,&bu8,1,&BytesRead);
				poke(addr,bu8);addr++;
			}
		}
		fclose(MyFile);
		RETN();
		return 1;

	}

	int version = 1;
	int isCompressed = 0;
	MEMPTR = 0;
	tstates = 11200;
	interrupts_enabled_at = 0;
	f_read_b(&MyFile,&bu8,1,&BytesRead); A = bu8;
	f_read_b(&MyFile,&bu8,1,&BytesRead); F = bu8;
	f_read_b(&MyFile,&BC,2,&BytesRead);
	f_read_b(&MyFile,&HL,2,&BytesRead);
	f_read_b(&MyFile,&PC,2,&BytesRead);
	if(PC==0)
	{
		version = 2;
	}
	f_read_b(&MyFile,&SP,2,&BytesRead);
	f_read_b(&MyFile,&bu8,1,&BytesRead); I = bu8;
	f_read_b(&MyFile,&bu8,1,&BytesRead); R = bu8;
	f_read_b(&MyFile,&bu8,1,&BytesRead); R|=(bu8&1)<<7;
	R7=R;
	if(bu8&(1<<5))
	{
		isCompressed = 1;
	}
	printf("Version %d ,isCompressed=%d\r\n",version,isCompressed);
	f_read_b(&MyFile,&DE,2,&BytesRead);
	f_read_b(&MyFile,&BC_,2,&BytesRead);
	f_read_b(&MyFile,&DE_,2,&BytesRead);
	f_read_b(&MyFile,&HL_,2,&BytesRead);
	f_read_b(&MyFile,&bu8,1,&BytesRead); A_ = bu8;
	f_read_b(&MyFile,&bu8,1,&BytesRead); F_ = bu8;
	f_read_b(&MyFile,&IY,2,&BytesRead);
	f_read_b(&MyFile,&IX,2,&BytesRead);
	f_read_b(&MyFile,&bu8,1,&BytesRead); IFF1 = bu8;
	f_read_b(&MyFile,&bu8,1,&BytesRead); IFF2 = bu8;
	f_read_b(&MyFile,&bu8,1,&BytesRead); IM   = bu8&3;
	halt = 0;
	if(version>1)
	{
		int b128 = 0;
		f_read_b(&MyFile,&bu16,2,&BytesRead);
		printf("Add Length=%d\r\n",bu16);
		if(bu16 == 23)
		{
			version = 2;
		}
		else if(bu16 == 54)
		{
			version = 3;
		}
		else if(bu16 == 55)
		{
			version = 4;
		}

		f_read_b(&MyFile,&bu16,2,&BytesRead); PC = bu16;
		f_read_b(&MyFile,&bu8,1,&BytesRead);
		if(bu8>1)
		{
			//?128
			b128 = 1;
			SINCLAIR_FLAGS.flag128 = 1;
			initRamSpec();
		}
		else
		{
			b128 = 0;
			SINCLAIR_FLAGS.flag128 = 0;
			initRamSpec();
		}
		f_read_b(&MyFile,&bu8,1,&BytesRead); printf("35=%d\r\n",bu8);
		if(b128)
		{
			setMemPages(bu8);
		}
		f_read_b(&MyFile,&bu8,1,&BytesRead); printf("36=%d\r\n",bu8);
		f_read_b(&MyFile,&bu8,1,&BytesRead); printf("37=%d\r\n",bu8);
		f_read_b(&MyFile,&bu8,1,&BytesRead); printf("38=%d\r\n",bu8);
		char sound[0x10];
		f_read_b(&MyFile,sound,0x10,&BytesRead);
		for(int k=0;k<0x10;k++)
		{
			printf("sound[%02d] %x\r\n",k,(unsigned)sound[k]);
		}
		if(version>=3)
		{
			f_read_b(&MyFile,&bu16,2,&BytesRead);  printf("LOW_T %x\r\n",bu16);
			f_read_b(&MyFile,&bu8,1,&BytesRead);  printf("HIGH_T %x\r\n",bu8);
			f_read_b(&MyFile,&bu8,1,&BytesRead);  printf("58 %x\r\n",bu8);
			f_read_b(&MyFile,&bu8,1,&BytesRead);  printf("59 %x\r\n",bu8);
			f_read_b(&MyFile,&bu8,1,&BytesRead);  printf("60 %x\r\n",bu8);
			f_read_b(&MyFile,&bu8,1,&BytesRead);  printf("61 %x\r\n",bu8);
			f_read_b(&MyFile,&bu8,1,&BytesRead);  printf("62 %x\r\n",bu8);
			f_read_b(&MyFile,sound,10,&BytesRead); printf("63 keyb[10]=\r\n");
			for(int k=0;k<10;k++)
			{
				printf("keyb[%02d] %x\r\n",k,(unsigned)sound[k]);
			}
			f_read_b(&MyFile,sound,10,&BytesRead); printf("73 ascii[10]=\r\n");
			for(int k=0;k<10;k++)
			{
				printf("ascii[%02d] %x\r\n",k,(unsigned)sound[k]);
			}
			
			f_read_b(&MyFile,&bu8,1,&BytesRead);  printf("83 %x\r\n",bu8);
			f_read_b(&MyFile,&bu8,1,&BytesRead);  printf("84 %x\r\n",bu8);
			f_read_b(&MyFile,&bu8,1,&BytesRead);  printf("85 %x\r\n",bu8);
			
			if(version==4)
			{
				f_read_b(&MyFile,&bu8,1,&BytesRead);  printf("86 %x\r\n",bu8);
			}
			
		}
		for(int p=0;p<8;p++)
		{
			f_read_b(&MyFile,&bu16,2,&BytesRead);printf("size= %x\r\n",bu16);
			f_read_b(&MyFile,&bu8,1,&BytesRead);  printf("page %x\r\n",bu8);
			int addr = 0x0000;
			int page = -1;
			printf("b128 %x\r\n",b128);
			if(!SINCLAIR_FLAGS.flag128)
			{
				b128 = 0;
			}
			if(!b128)
			{
				
				addr = 0;
				if(bu8==4)
				{
					addr = 0x8000;
					page = 2;
				}
				else if(bu8==5)
				{
					addr = 0xc000;
					page = 0;
				}
				else if(bu8==8)
				{
					addr = 0x4000;
					page = 5;
				}
				else page = -1;
			}
			else
			{
				page =  bu8-3;
				addr  = 0;
				if(bu8==3)
				{
					addr = 0xc000;
				}
				else if(bu8==5)
				{ 
					addr = 0x8000;
				}
				else if(bu8==8)
				{
					addr = 0x4000;
				}
				
			}
			if(page>=0&&page<=7)
			{
				if(bu16==0xffff)
				{
					if(page>=0)
					{
						for(int k=0;k<0x4000;k++)
						{
							f_read_b(&MyFile,&bu8,1,&BytesRead); 
							poke_page(page,addr,bu8);addr++;
						}
					}
				}
				else
				{
					if(page>=0)
					{
						int bytes = 0;
						uint8_t rl;
						while(bytes<bu16)
						{
							f_read_b(&MyFile,&bu8,1,&BytesRead);  bytes++;
							if(bu8==0xed)
							{
								f_read_b(&MyFile,&bu8,1,&BytesRead);  bytes++;
								if(bu8==0xed)
								{
									//ifs.read((char*)&rl,1); 
									//ifs.read((char*)&bu8,1); bytes++;
									f_read_b(&MyFile,&rl,1,&BytesRead);bytes++;
									f_read_b(&MyFile,&bu8,1,&BytesRead);  bytes++;
									for(int k=0;k<rl;k++)
									{
										poke_page(page,addr,bu8);addr++;
									}
								}
								else
								{
									poke_page(page,addr,0xed);
									addr++;
									poke_page(page,addr,bu8);
									addr++;
								}
							}
							else 
							{
								poke_page(page,addr,bu8);
								addr++;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		SINCLAIR_FLAGS.flag128 = 0;
		printf("Force to 48 Version !\r\n");
		initRamSpec();
		int addr = 0x4000;
		uint8_t rl;
		if(!isCompressed)
		{
			//read 48K raw
			for(int k=0;k<0x10000-0x4000;k++)
			{
				//ifs.read((char*)&bu8,1); 
				f_read_b(&MyFile,&bu8,1,&BytesRead); 
				poke(addr,bu8);addr++;
			}
		}
		else
		{
			while(addr<0x10000)
			{
				//ifs.read((char*)&bu8,1); 
				f_read_b(&MyFile,&bu8,1,&BytesRead); 
				if(BytesRead)
				{
					if(bu8==0xed)
					{
						//ifs.read((char*)&bu8,1); 
						f_read_b(&MyFile,&bu8,1,&BytesRead); 
						if(bu8==0xed)
						{
							f_read_b(&MyFile,&rl,1,&BytesRead); 
							f_read_b(&MyFile,&bu8,1,&BytesRead); 
			//				ifs.read((char*)&rl,1); 
			//				ifs.read((char*)&bu8,1); 
							for(int k=0;k<rl;k++)
							{
								poke(addr,bu8);addr++;
							}
						}
						else
						{
							poke(addr,0xed);
							addr++;
							poke(addr,bu8);
							addr++;
						}
					}
					else 
					{
						poke(addr,bu8);
						addr++;
					}
				}
				else
				{
					printf("addr %x\r\n",addr);
					addr = 0x10000;
				}
					
			}
		}
		//PRINT(version);
		//PRINT(isCompressed);
		fclose(MyFile);
		return 1;
	}
	
	fclose(MyFile);
	//f_mount(NULL,USERPath, 0);
	//FATFS_UnLinkDriver(USERPath);
	return 1;
	}
	else
	{
		return 0;
	}


}




int       menuLines = 0;
#define   NUM_MENU_LINES
enum  MENU_LINES
{
	MenuJoystik=0,
	MenuDiplayBrightColor,
	MenuTapeSpeed,
	MenuSaveGame,
	MenuExitFromSimulator,
	MenuLast
};
// child nodes
char* BRIGHTNESS = "BRIGHT";
char* SAVE  = "Save Game";
char* TAPE = "TAPE %d"; //0 dis
char* EXIT 		 = "Exit to Menu";

int colorTable[2][3] =
{
		{LGRAY,DGRAY,BLACK},
		{GREEN,YELLOW,BLACK}
};
int getMenuItemColor(int current)
{
	return (current==menuLines)?colorTable[SINCLAIR_FLAGS.bColor][0]:colorTable[SINCLAIR_FLAGS.bColor][1];
}
int getC(int current)
{
	if(current==menuLines) return BLACK;
	else return WHITE;
}
int getB(int current)
{
	if(current!=menuLines) return BLACK;
	else return WHITE;
}
#include "fonts.h"
int z48_z128_dialog_dispatch(struct SYS_EVENT* ev)
{
	Delay(2);
	char sBuff[0x10];
	if(ev->message)
	{
		printf("z48_z128_dialog_dispatch  = %x\r\n",ev->message);
	}
	if(ev->message==MESS_OPEN)
	{
		cleanAudioBuffer();
		ev->message = MESS_REPAINT;
		return 0;
	}
	else if(ev->message==MESS_REPAINT)
	{
#define LINE_SHIFT  (CHAR_HEIGHT/2+2)
#define LR_SHIFT  (CHAR_WIDTH/2)

		const uint16_t yScr = LCD_getHeight()/2-LINE_SHIFT*8;
		const uint16_t xScr = LCD_getWidth()/2;

#ifdef TV_OUT
		LCD_Draw_Text(JS_LINES[SINCLAIR_FLAGS.joystickMode],xScr-strlen(JS_LINES[SINCLAIR_FLAGS.joystickMode])*LR_SHIFT,yScr+LINE_SHIFT, getC(MenuJoystik), 1,getB(MenuJoystik));
		LCD_Draw_Text(BRIGHTNESS,xScr-strlen(BRIGHTNESS)*LR_SHIFT,yScr+LINE_SHIFT*3, getC(MenuDiplayBrightColor), 1,getB(MenuDiplayBrightColor));
		sprintf(sBuff,TAPE,SINCLAIR_FLAGS.tapeSpeed);
		LCD_Draw_Text(sBuff,xScr-strlen(sBuff)*LR_SHIFT		,yScr+LINE_SHIFT*5, getC(MenuTapeSpeed), 1,getB(MenuTapeSpeed));
		LCD_Draw_Text(SAVE,xScr-strlen(SAVE)*LR_SHIFT		 	,yScr+LINE_SHIFT*7, getC(MenuSaveGame), 1,getB(MenuSaveGame));
		LCD_Draw_Text(EXIT,xScr-strlen(EXIT)*LR_SHIFT			,yScr+LINE_SHIFT*9, getC(MenuExitFromSimulator), 1,getB(MenuExitFromSimulator));
		char* Map =joyMap[SINCLAIR_FLAGS.joystickMode];

		LCD_Draw_Char(Map[0],xScr-LR_SHIFT   ,yScr+LINE_SHIFT*11,BLACK,1,WHITE);
		LCD_Draw_Char(Map[2],xScr-LR_SHIFT-48,yScr+LINE_SHIFT*13,BLACK,1,WHITE);
		LCD_Draw_Char(Map[3],xScr-LR_SHIFT+48,yScr+LINE_SHIFT*13,BLACK,1,WHITE);
		LCD_Draw_Char(Map[4],xScr-LR_SHIFT-96,yScr+LINE_SHIFT*15,BLACK,1,WHITE);
		LCD_Draw_Char(Map[5],xScr-LR_SHIFT-48,yScr+LINE_SHIFT*15,BLACK,1,WHITE);
		LCD_Draw_Char(Map[1],xScr-LR_SHIFT   ,yScr+LINE_SHIFT*15,BLACK,1,WHITE);
#else
		LCD_Draw_Text(JS_LINES[SINCLAIR_FLAGS.joystickMode],xScr-strlen(JS_LINES[SINCLAIR_FLAGS.joystickMode])*LR_SHIFT,yScr+LINE_SHIFT, BLACK, 1,getMenuItemColor(MenuJoystik));
		LCD_Draw_Text(BRIGHTNESS,xScr-strlen(BRIGHTNESS)*LR_SHIFT,yScr+LINE_SHIFT*3, BLACK, 1,getMenuItemColor(MenuDiplayBrightColor));
		sprintf(sBuff,TAPE,SINCLAIR_FLAGS.tapeSpeed);
		LCD_Draw_Text(sBuff,xScr-strlen(sBuff)*LR_SHIFT		,yScr+LINE_SHIFT*5, BLACK, 1,getMenuItemColor(MenuTapeSpeed));
		LCD_Draw_Text(SAVE,xScr-strlen(SAVE)*LR_SHIFT		 	,yScr+LINE_SHIFT*7, BLACK, 1,getMenuItemColor(MenuSaveGame));
		LCD_Draw_Text(EXIT,xScr-strlen(EXIT)*LR_SHIFT			,yScr+LINE_SHIFT*9, BLACK, 1,getMenuItemColor(MenuExitFromSimulator));
		char* Map =joyMap[SINCLAIR_FLAGS.joystickMode];

		LCD_Draw_Char(Map[0],xScr-LR_SHIFT   ,yScr+LINE_SHIFT*11,BLACK,1,colorTable[SINCLAIR_FLAGS.bColor][1]);
		LCD_Draw_Char(Map[2],xScr-LR_SHIFT-48,yScr+LINE_SHIFT*13,BLACK,1,colorTable[SINCLAIR_FLAGS.bColor][1]);
		LCD_Draw_Char(Map[3],xScr-LR_SHIFT+48,yScr+LINE_SHIFT*13,BLACK,1,colorTable[SINCLAIR_FLAGS.bColor][1]);
		LCD_Draw_Char(Map[4],xScr-LR_SHIFT-96,yScr+LINE_SHIFT*15,BLACK,1,colorTable[SINCLAIR_FLAGS.bColor][1]);
		LCD_Draw_Char(Map[5],xScr-LR_SHIFT-48,yScr+LINE_SHIFT*15,BLACK,1,colorTable[SINCLAIR_FLAGS.bColor][1]);
		LCD_Draw_Char(Map[1],xScr-LR_SHIFT   ,yScr+LINE_SHIFT*15,BLACK,1,colorTable[SINCLAIR_FLAGS.bColor][1]);
		//~ LCD_Draw_Text(JS_LINES[SINCLAIR_FLAGS.joystickMode],xScr-strlen(JS_LINES[SINCLAIR_FLAGS.joystickMode])*CHAR_WIDTH,yScr+LINE_SHIFT, BLACK, 2,getMenuItemColor(MenuJoystik));
		//~ LCD_Draw_Text(BRIGHTNESS,xScr-strlen(BRIGHTNESS)*CHAR_WIDTH,yScr+LINE_SHIFT*3, BLACK, 2,getMenuItemColor(MenuDiplayBrightColor));
		//~ sprintf(sBuff,TAPE,SINCLAIR_FLAGS.tapeSpeed);
		//~ LCD_Draw_Text(sBuff,xScr-strlen(sBuff)*CHAR_WIDTH		,yScr+LINE_SHIFT*5, BLACK, 2,getMenuItemColor(MenuTapeSpeed));
		//~ LCD_Draw_Text(SAVE,xScr-strlen(SAVE)*CHAR_WIDTH		 	,yScr+LINE_SHIFT*7, BLACK, 2,getMenuItemColor(MenuSaveGame));
		//~ LCD_Draw_Text(EXIT,xScr-strlen(EXIT)*CHAR_WIDTH			,yScr+LINE_SHIFT*9, BLACK, 2,getMenuItemColor(MenuExitFromSimulator));
		//~ char* Map =joyMap[SINCLAIR_FLAGS.joystickMode];

		//~ LCD_Draw_Char(Map[0],xScr-CHAR_WIDTH   ,yScr+LINE_SHIFT*11,BLACK,2,colorTable[SINCLAIR_FLAGS.bColor][1]);
		//~ LCD_Draw_Char(Map[2],xScr-CHAR_WIDTH-48,yScr+LINE_SHIFT*13,BLACK,2,colorTable[SINCLAIR_FLAGS.bColor][1]);
		//~ LCD_Draw_Char(Map[3],xScr-CHAR_WIDTH+48,yScr+LINE_SHIFT*13,BLACK,2,colorTable[SINCLAIR_FLAGS.bColor][1]);
		//~ LCD_Draw_Char(Map[4],xScr-CHAR_WIDTH-96,yScr+LINE_SHIFT*15,BLACK,2,colorTable[SINCLAIR_FLAGS.bColor][1]);
		//~ LCD_Draw_Char(Map[5],xScr-CHAR_WIDTH-48,yScr+LINE_SHIFT*15,BLACK,2,colorTable[SINCLAIR_FLAGS.bColor][1]);
		//~ LCD_Draw_Char(Map[1],xScr-CHAR_WIDTH   ,yScr+LINE_SHIFT*15,BLACK,2,colorTable[SINCLAIR_FLAGS.bColor][1]);
#endif
		Delay(3);
		clearKey();
		ev->message = MESS_IDLE;
		return 0;
	}
	else if(ev->message==MESS_IDLE)
	{

	}
	else if(ev->message==MESS_KEYBOARD)
	{
		if(ev->param1==K_ESC)
		{
			setAttr();
			Delay(2);
			clearKey();
			ev->message = MESS_CLOSE;
			return 0;
		}
		else if(ev->param1==K_FIRE)
		{
//			MenuJoystik=0,
//			MenuDiplayBrightColor,
//			MenuTapeSpeed,
//			MenuExitFromSimulator,

			if(menuLines==MenuExitFromSimulator)
			{
				tape_close();
				cleanAudioBuffer();
				ev->message = MESS_CLOSE;
				popMenuFunc();
				return 0;
			}
			else if(menuLines==MenuDiplayBrightColor)
			{
				SINCLAIR_FLAGS.bColor = !SINCLAIR_FLAGS.bColor;
				ev->message = MESS_REPAINT;
				Delay(2);
				clearKey();
				return 1;
			}
			else
			{
				setAttr();
				Delay(2);
				clearKey();
				ev->message = MESS_CLOSE;
				return 0;
			}
		}
		else if(ev->param1==K_UP)
		{
			menuLines =( menuLines+MenuLast-1)%MenuLast;
		}
		else if(ev->param1==K_DOWN)
		{
			menuLines =( menuLines+1)%MenuLast;
		}
		else if(ev->param1==K_RIGHT)
		{
			if(menuLines==MenuJoystik)
			{
				SINCLAIR_FLAGS.joystickMode = (SINCLAIR_FLAGS.joystickMode+joyMapSize-1)%joyMapSize;
			}
			else if(menuLines==MenuDiplayBrightColor)
			{
				setBacklight(getBacklight()+5);
			}
			else if(menuLines==MenuTapeSpeed)
			{
				SINCLAIR_FLAGS.tapeSpeed =(SINCLAIR_FLAGS.tapeSpeed+1)%3;
				tapeSetTime();
			}
			else if(menuLines==MenuExitFromSimulator)
			{

			}
		}
		else  if(ev->param1==K_LEFT)
		{
			if(menuLines==MenuJoystik)
			{
				SINCLAIR_FLAGS.joystickMode = (SINCLAIR_FLAGS.joystickMode+1)%joyMapSize;
			}
			else if(menuLines==MenuDiplayBrightColor)
			{
				setBacklight(getBacklight()-5);
			}
			else if(menuLines==MenuTapeSpeed)
			{
				SINCLAIR_FLAGS.tapeSpeed =(SINCLAIR_FLAGS.tapeSpeed+2)%3;
				tapeSetTime();
			}
			else if(menuLines==MenuExitFromSimulator)
			{

			}
		}
		ev->message = MESS_REPAINT;
		return 1;
	}
	ev->message=MESS_IDLE;
	return 1;
}
extern const uint16_t BKLC[512][2];

void LCD_Write8x8line16(uint16_t x1,uint16_t y1,uint16_t * data);
int  tbSetTape = -1;
void setTapeSpeedImp()
{
	int ts = tbSetTape;
	if(ts != SINCLAIR_FLAGS.tapeSpeed)
	{
		if(ts!=-1)
		{
			if(SINCLAIR_FLAGS.tapeSpeed==0)
			{
					SINCLAIR_FLAGS.frameCounter = SINCLAIR_FLAGS.tape_frameCounter;
					tapeStart();
					tapeSetTime();
			}
			else
			{
					if(ts==0)
					{
						tapeStop();
						SINCLAIR_FLAGS.tape_frameCounter =SINCLAIR_FLAGS.frameCounter;
					}
			}
			SINCLAIR_FLAGS.tapeSpeed = ts;
		}
	}
	tbSetTape = -1;
}

void setTapeSpeed(int ts)
{
	tbSetTape = ts;
}
void setScreenOffset(int k)
{
	setAttr();

	SINCLAIR_FLAGS.screenOffset = k;
}
int z48_z128_dispatch(struct SYS_EVENT* ev)
{
	//if(ev->message)
	{
		//printf("z48_z128_dispatch  = %x\r\n",ev->message);
	}
	if(ev->message==MESS_OPEN)
	{
		if(getModeFile()==0)
		{
			SINCLAIR_FLAGS.flag128 = 0;
		}
		else
		{
			DirtyMemorySet();
			SINCLAIR_FLAGS.flag128 = 1;
		}
		initRamSpec();
		if(getFileType(getFileName())==T_TAP)
		{
			TapeState_constructor();
			if(tape_open(getFileName()))
			{
				clearFullScreen();
				initRamSpec();
				ay_reset();
				z80_reset(1);
				SKEY_QUE_init();
				printf("SKEY_QUE_size()=%d\r\n",SKEY_QUE_size());
				int TM_OUT= 48;
				if(SINCLAIR_FLAGS.flag128)
				{
					SINCLAIR_FLAGS.tapeSpeed = 1;

					SKEY_QUE_put(ENTER,0,TM_OUT*4);

					SKEY_QUE_put(ENTER,1,TM_OUT);
					SKEY_QUE_put(ENTER,0,TM_OUT);
				}
				else
				{
					SINCLAIR_FLAGS.tapeSpeed = 1;

					SKEY_QUE_put(SKEY_J,0,TM_OUT*4);

					SKEY_QUE_put(SKEY_J,1,TM_OUT);
					SKEY_QUE_put(SKEY_J,0,TM_OUT);
					SKEY_QUE_put(SYMB_SHIFT,1,TM_OUT);
					SKEY_QUE_put(SKEY_P,1,TM_OUT);
					SKEY_QUE_put(SKEY_P,0,TM_OUT);
					SKEY_QUE_put(SKEY_P,1,TM_OUT);
					SKEY_QUE_put(SKEY_P,0,TM_OUT);
					SKEY_QUE_put(SYMB_SHIFT,0,TM_OUT);
					SKEY_QUE_put(ENTER,1,TM_OUT);
					SKEY_QUE_put(ENTER,0,TM_OUT);
				}

				printf("SKEY_QUE_size()=%d\r\n",SKEY_QUE_size());
				//SKEY_QUE_put(((14-8)<<8)|0);
				tapeStart();
				tapeSetTime();
				ev->message = MESS_REPAINT;
				return 1;
			}
			else
			{
				ev->message = MESS_CLOSE;
				return 1;
			}
		}
		else if(getFileType(getFileName())==T_Z80)
		{
			initRamSpec();
			ay_reset();
			SKEY_QUE_init();
			z80_reset(1);
			DirtyMemorySet();
			printf("full name%s\r\n",getFileName());
			if(!freadFile_vers(getFileName()))
			{
				ev->message = MESS_CLOSE;
				return 0;
			}
			else
			{
				setAttr();
				ev->message = MESS_REPAINT;
				return 1;
			}

		}
		ev->message=MESS_IDLE;
		return 1;
	}
	else if(ev->message==MESS_IDLE||ev->message==MESS_KEYBOARD)
	{
		if(ev->message==MESS_KEYBOARD)
		{
			if(ev->param1==K_ESC)
			{
				setCurrentFunc(z48_z128_dialog_dispatch);
//				tape_close();
//				cleanAudioBuffer();
//				ev->message = MESS_CLOSE;
				ev->message = MESS_OPEN;
				return 0;
			}
		}
		int32_t tickstart = HAL_GetTick();
		kkeyScanN = 0;
		for(int line = 0;line< TAPE_LINES;line++)
		{
			SINCLAIR_FLAGS.borderArray[line].color_curr=0xf;
		}
		soundEvents(100);
		{
			//z80_interrupt();
			for(int tt = 0;tt<SINCLAIR_FLAGS.T69888;tt+=1000)
			{
				z80_run(tt);
				//kscan0();
				procKeyb();
				 // new lines prevent sound buffer overflow
#if 1
				if(aSOUND_EVENTS_size()>SOUND_EVENT_ARRAY_SIZE/2)
				{
					proccesSoundEvents_time();
					soundEvents(100);
				}
#endif
			}
				z80_run(SINCLAIR_FLAGS.T69888);
			}
		setTapeSpeedImp();
		uint32_t cputime = HAL_GetTick() - tickstart;
		uint32_t sound_events = aSOUND_EVENTS_size();
		proccesSoundEvents_time();
		static int cntt = 0;
		cntt++;
		static uint32_t prev_tick = 0;
		uint32_t curr_tick = HAL_GetTick();
		if((cntt&0x1f)==1)
		{
			printf("tt=%d %d %d %d\r\n",cputime,(curr_tick-prev_tick)/0x1f,sound_events,kkeyScanN);
			prev_tick = curr_tick;
		}

		tstates -=SINCLAIR_FLAGS.T69888;
		interrupts_enabled_at -=SINCLAIR_FLAGS.T69888;
		SINCLAIR_FLAGS.frameCounter++;
		ev->message=MESS_REPAINT;
		return 1;
	}
	else if(ev->message==MESS_REPAINT)
	{

#define SPECTRUM_HEIGHT  192
#define SPECTRUM_WIDTH   256
//#define SCALE 1
		int OffSX = (LCD_getWidth()  -SPECTRUM_WIDTH)/2;
		int OffSY = (LCD_getHeight()  -SPECTRUM_HEIGHT)/2;
	//  if((SINCLAIR_FLAGS.frameCounter%2)==0)
	{
//		static uint16_t old_border =0xffff;
		//~ HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
		//~ if(old_border!=(border&0x7))
		//~ {
			//~ old_border = border&0x7;
			//~ const uint16_t yScr = (240-192)/2;
			//~ const uint16_t xScr = (320-256)/2;
			//~ uint8_t buff[3];
			//~ buff[0] = 0xd7*(!!(border&2));
			//~ buff[1] = 0xd7*(!!(border&4));
			//~ buff[2] = 0xd7*(!!(border&1));
			//~ LCD_FullRect3(xScr-4,yScr-4,buff,4,192+8) ;
			//~ LCD_FullRect3(xScr+256,yScr-4,buff,4,192+8) ;
			//~ LCD_FullRect3(xScr-4,yScr-4,buff,256+8,4) ;
			//~ LCD_FullRect3(xScr-4,yScr+192,buff,256+8,4) ;
		//~ }

		// x tile_ofs = x (0,32)
		// y tile_ofs
		SINCLAIR_FLAGS.screenOffset = 0;
		int yb,xb,xbb;
		for( yb=0;yb<24;yb++)
		{
			for(xbb=0;xbb<32;xbb++)
			{
				xb = SINCLAIR_FLAGS.screenOffset+xbb;
				int tileAddr = xb+yb*32;
				//if(CommonData.SincData.ATTR_RAM_MOD[tileAddr>>3] & (1<<(tileAddr&0x3)) || (ATTR_RAM[tileAddr]&128))
				{
					//~ ATTR_RAM_MOD[tileAddr>>3] &= 0xff-(1<<(tileAddr&0x3));
					uint8_t attr = ATTR_RAM[tileAddr];
					//uint16_t* pp = &BKLC[attr|(SINCLAIR_FLAGS.frameCounter&32)*8][0];
					uint8_t Br = !!(attr&64);
					int scale = Br?3:2;
					colorType FG = MCOLOR(scale*!!(attr&16),scale*!!(attr&32),scale*!!(attr&8));
					colorType BG = MCOLOR(scale*!!(attr&2),scale*!!(attr&4),scale*!!(attr&1));
					if((attr&128)&&(SINCLAIR_FLAGS.frameCounter&32))
					{
						colorType Temp = FG;
						FG=BG;
						BG= Temp;
					}
					int yt;
					int xScr = xbb*8+OffSX;
					int yScr = yb*8+OffSY;
					colorType buff[8][8];
					for(yt=0;yt<8;yt++)
					{
						int y = yt+yb*8;
						//~ uint8_t lineColor[32];
						uint16_t vr = ((y<<8)&0x0700)|((y<<2)&0xe0)|((y<<5)&0x1800);
						uint8_t   val  = VIDEO_RAM[vr+xb];
						int bit;
						if(SINCLAIR_FLAGS.bColor)
						{
							
						}
						else
						{
						}
						for(int bit=0;bit<8;bit++)
						{
							buff[yt][bit]= ((val>>(7-bit))&1)?BG:FG;
							//LCD_fillRect(xScr+bit,yScr+yt,1,1,((val>>(7-bit))&1)?BG:FG);
						//LCD_fillRect(xScr+bit,yScr+yt,1,1,WHITE);
						}
					}
					LCD_fillRectP(xScr,yScr,8,8,buff[0]);
				}
			}
		}
		procKeyb();
		clearAttr();
		}
		{
			const uint16_t yScr = BORDER_HEIGHT;
			uint8_t bord = SINCLAIR_FLAGS.border;
			for(int line = 0;line< TAPE_LINES;line++)
			{
				procKeyb();
				if(SINCLAIR_FLAGS.borderArray[line].color_curr != 0xf)
				{
					bord = SINCLAIR_FLAGS.borderArray[line].color_curr;
				}
				if(bord!=SINCLAIR_FLAGS.borderArray[line].color_prev)
				{
					SINCLAIR_FLAGS.borderArray[line].color_prev = bord;
					int scale = 2;
					uint8_t attr  = bord;
					colorType color = MCOLOR(scale*!!(attr&2),scale*!!(attr&4),scale*!!(attr&1));
					if(!SINCLAIR_FLAGS.bColor)
					{
					}
					if((line<yScr)||(line>=yScr+SPECTRUM_HEIGHT))
					{
						LCD_fillRect(OffSX-BORDER_WIDTH,OffSY-BORDER_HEIGHT+line,SPECTRUM_WIDTH+BORDER_WIDTH*2,1,color);
					}
					else
					{
						LCD_fillRect(OffSX-BORDER_WIDTH,OffSY-BORDER_HEIGHT+line,BORDER_WIDTH,1,color);
						LCD_fillRect(OffSX+SPECTRUM_WIDTH,OffSY-BORDER_HEIGHT+line,BORDER_WIDTH,1,color);
					}
					//else if(getTapeState())
					//{
						//LCD_fillRect(OffSX-BORDER_WIDTH,line,12,1,color);
						//LCD_fillRect(240-12,line,12,1,color);
					//}
				}
			}
			//SINCLAIR_FLAGS.borderArray[line]=0xff;
		}
		
		ev->message=MESS_IDLE;
		screen_IRQ = 1;
		return 1;
	}
	ev->message=MESS_IDLE;
	return 0;
}


