#ifndef __MAIN_H
#define __MAIN_H
#include <stdint.h>
enum {
	K_UP    = 0x100,
	K_DOWN  = 0x4,
	K_LEFT  = 0x10,
	K_RIGHT = 0x40,
	K_ESC   = 0x200,
	K_FIRE  = 0x8,
	K_SPACE = 0x20,
	K_TOUCH = 0x400
};
enum //messages
{
	MESS_IDLE     = 0,
	MESS_KEYBOARD,
	MESS_OPEN,
	MESS_REPAINT,
	MESS_CLOSE
};

enum {
	T_UNKNOWN=0,
	T_TAP,
	T_Z80,
	T_MP3,
	T_NES
};

struct SYS_EVENT
{
	uint16_t message;
	uint16_t param1;
	uint16_t param2;
	void      *data;
};
typedef int  (*dispathFunction)(struct SYS_EVENT* ev);
void pushMenuFunc(void*fnk);
dispathFunction popMenuFunc();

typedef struct 
{
  uint8_t state;
  uint8_t lctrl;
  uint8_t lshift;
  uint8_t lalt;
  //~ uint8_t lgui;
  uint8_t rctrl;
  uint8_t rshift;
   uint8_t ralt;
  //~ uint8_t rgui;
  uint8_t keys[6];
} MY_HID_KEYBD_Info_TypeDef;


void Delay(int ms);
uint32_t  HAL_GetTick();
void kscan0();

int getBacklight();
void setBacklight(int bl);
void TapeState_constructor();

#define MOUNT_POINT "/sdcard"
int readDirIntoList(char* currentDir);
void setCurrentFunc(void*fnk);
void initStrList();
int getFileType(char* name);
void putStr(uint8_t attr,char* str);
int setModeFile(int mode);
void init_Sinclair();
void clearKey();
int menu_dispatch(struct SYS_EVENT* ev);
int z48_z128_dispatch(struct SYS_EVENT* ev);
void setTapeSpeed(int);
void setScreenOffset(int);
void ay_reset();
void z80_reset();
void SKEY_QUE_init();
void cleanAudioBuffer();
void initRamSpec();
void initSDCard();

#define MAX_VOLUME 255
#endif