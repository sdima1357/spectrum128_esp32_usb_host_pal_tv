#include "main.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int nes_dispatch(struct SYS_EVENT* ev)
{
	return 0;
}

char baseDirA[0x20];
int  rfilesA ;
char fnameBuff[0x100];
int  modeFile  = 0;
int  baseline = 0;
int  selection = 0;
int  invalidMemory = 0;
int z48_z128_dialog_dispatch(struct SYS_EVENT* ev);
int mp3_play_dispatch(struct SYS_EVENT* ev);
int z48_z128_dispatch(struct SYS_EVENT* ev);
int isDirtyMemory();
int getModeFile()
{
	return modeFile;
}
int setModeFile(int mode)
{
	modeFile = mode;
	return modeFile;
}

int isDirtyMemory()
{
	return invalidMemory;
}
void DirtyMemoryClear()
{
	invalidMemory = 0;
}
void DirtyMemorySet()
{
	invalidMemory = 1;
}

uint8_t * getRamBuffer0x4000(int k);
void clearKey();

int getFileType(char* name)
{
	int type = T_UNKNOWN;
	if(strcasestr(name,".tap"))
	{
		type = T_TAP;
	}
	else if(strcasestr(name,".z80"))
	{
		type = T_Z80;
	}
	else if(strcasestr(name,".sna"))
	{
		type = T_Z80;
	}
	else if(strcasestr(name,".mp3"))
	{
		type = T_MP3;
	}
	else if(strcasestr(name,".nes"))
	{
		type = T_NES;
	}
	return type;
}


char* getFileName()
{
	return fnameBuff;
}
#define MAX_FILES 320
typedef uint8_t* p_uint8;
p_uint8* strListArray;
int      strCounter;
uint8_t* currAllock;
void initStrList()
{
	currAllock  = getRamBuffer0x4000(6);
	strListArray = currAllock;
	currAllock  += sizeof(p_uint8)*MAX_FILES;
	strCounter = 0;
}
void putStr(uint8_t attr,char* str)
{
	if(strCounter<MAX_FILES-1)
	{
		int rlen = strlen(str);
		strListArray[strCounter] = currAllock;
		currAllock += rlen+2;
		*strListArray[strCounter] = attr;
		memcpy(strListArray[strCounter]+1,str,rlen+1);
		strCounter++;
	}

}

#include "fonts.h"
int file_dispatch(struct SYS_EVENT* ev)
{
	Delay(2);
	const int NUmL =  LCD_getHeight()/CHAR_HEIGHT-1;
	if(ev->message)
	{
		//printf("file_dispatch  = %x\r\n",ev->message);
	}
	if(ev->message==MESS_OPEN)
	{
		modeFile = ev->param1;
		clearFullScreen();
		printf("get card init ok\r\n");

#if 0
		BSP_SD_CardInfo CardInfo;
		BSP_SD_GetCardInfo(&CardInfo);
		printf("Class: 		%d\r\n",CardInfo.Class);
		printf("CardType: 	%d\r\n",CardInfo.CardType);
		printf("CardVersion: 	%d\r\n",CardInfo.CardVersion);
		printf("BlockNbr: 	%d\r\n",CardInfo.BlockNbr);
		printf("BlockSize:	%d\r\n",CardInfo.BlockSize);
		printf("LogBlockNbr:	%d\r\n",CardInfo.LogBlockNbr);
		printf("LogBlockSize:	%d\r\n",CardInfo.LogBlockSize);
		printf("RelCardAdd:	%d\r\n",CardInfo.RelCardAdd);
		rfilesA = 0;
		f_mount(NULL,SDPath, 0);
		if(f_mount(&SDFatFS,SDPath, 0) != FR_OK)
		{
			printf("f_mount Error!\r\n");
			ev->message = MESS_CLOSE;
			return 0;
		}
#endif		
		rfilesA = readDirIntoList(baseDirA);
		DirtyMemoryClear();
		printf("rfilesA==%d modeFile = %d\r\n",rfilesA,modeFile);

		/* Check the mounted device */
		if(modeFile==3)
		{
			strcpy(baseDirA,"/NES");
		}
		else if(modeFile==2)
		{
			strcpy(baseDirA,"/MP3");
		}
		else if (modeFile==1)
		{
			strcpy(baseDirA,"/Z128");
		}
		else if (modeFile==0)
		{
			strcpy(baseDirA,"/Z48");
		}
		rfilesA = readDirIntoList(baseDirA);
		DirtyMemoryClear();
		printf("rfilesA==%d modeFile = %d\r\n",rfilesA,modeFile);
		ev->message = MESS_REPAINT;
		return 1;
	}
	else if(ev->message==MESS_REPAINT)
	{
		if(isDirtyMemory())
		{
			ev->message=MESS_OPEN;
			DirtyMemoryClear();
			ev->param1 = modeFile;
			return 1;
		}
		else
		{
			int k;
			//LCD_fillRect(0,0,LCD_getWidth(),LCD_getHeight(),BLACK);
			//printf("paint_ rfilesA %d  selection %d \r\n",rfilesA,selection);
			if(selection >= rfilesA)
			{
				selection-=(selection-rfilesA)+1;
				baseline = selection- NUmL+1;
				if(baseline<0) baseline = 0;
			}
			//printf("paint_ rfilesA %d  selection %d \r\n",rfilesA,selection);
			for(k = 0;k <NUmL;k++)
			{
				int n = baseline + k;
				const uint16_t yScr = 0;
				const uint16_t xScr = 0;
				int SYMB = LCD_getWidth()/CHAR_WIDTH-3;
				char buffer[SYMB+1];
				buffer[SYMB] = 0;
				memset(buffer,' ',SYMB);
				char * type[] = {"0:","T:","Z:","M:","N:"};
				LCD_Draw_Text("  ",xScr,yScr+(k)*CHAR_HEIGHT, WHITE, 1, BLACK);
				if(n<rfilesA)
				{
					char * pnt = (char*)&strListArray[n][1];
					for(int p=0;p<SYMB;p++)
					{
						if(*pnt)
						{
							buffer[p] = *pnt;
							pnt++;
						}
					}
					LCD_Draw_Text(type[strListArray[n][0]],xScr,yScr+(k)*CHAR_HEIGHT, (selection==n)?BLACK:WHITE, 1, (selection==n)?WHITE:BLACK);
				}
				LCD_Draw_Text(buffer,xScr+CHAR_WIDTH*3,yScr+(k)*CHAR_HEIGHT, (selection==n)?BLACK:WHITE, 1, (selection==n)?WHITE:BLACK);

			}
			Delay(100);
			clearKey();
			ev->message = MESS_IDLE;
			//~ char* fname = (char*)(strListArray[selection]+1);
			//printf("paint_ok rfilesA %d  selection %d %s\r\n",rfilesA,selection,fname);
			return 1;
		}
	}
	else if(ev->message==MESS_IDLE)
	{
		return 1;
	}
	else if(ev->message==MESS_KEYBOARD)
	{
		if(ev->param1==K_ESC)
		{

			ev->message = MESS_CLOSE;
			return 0;
		}
		else if(ev->param1==K_DOWN)
		{
			if(selection<rfilesA-1)
			{
				selection++;
			}
			if(selection-NUmL+1>baseline)
			{
				baseline++;
			}
			ev->message = MESS_REPAINT;
			return 0;
		}
		else if(ev->param1==K_UP)
		{
			if(selection>0)
			{
				selection--;
			}
			if(baseline>selection)
			{
				baseline--;
			}
			ev->message = MESS_REPAINT;
			return 0;
		}
		else if(ev->param1==K_FIRE) //fire
		{
			char* fname = (char*)(strListArray[selection]+1);
			printf("selected filename %s\r\n",strListArray[selection]+1);
			//selNum = selection;
			//make full filename
			strcpy(fnameBuff,MOUNT_POINT);
			strcpy(fnameBuff+strlen(fnameBuff),baseDirA);
			strcpy(fnameBuff+strlen(fnameBuff),"/");
			strcpy(fnameBuff+strlen(fnameBuff),fname);

			if((modeFile==0)||(modeFile==1))
			{
				setCurrentFunc(&z48_z128_dispatch);
			}
			else if(modeFile==3)
			{
				//nes loop;
				setCurrentFunc(&nes_dispatch);
			}
			else
			{
				setCurrentFunc(&mp3_play_dispatch);
			}
			ev->message = MESS_OPEN;
			return 0;
		}
		ev->message = MESS_IDLE;
		return 1;
	}
	ev->message = MESS_IDLE;
	return 1;
}

