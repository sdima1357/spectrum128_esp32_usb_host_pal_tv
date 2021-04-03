/*
 * tape.c

 *
 *  Created on: Feb 1, 2019
 *      Author: dima
 */
#include "main.h"

/* USER CODE BEGIN Includes */

extern int32_t tstates;
void soundEvents(int event);
void mprintf(const char *fmt, ...);

#include <stdio.h>

FILE* ifs_tape = 0;

//~ #define HAVE
#define FILE_BUFFER_SIZE 128
enum   eState {tNoFile=-1, tUndef=0,tHeadHeadPrefix,tHeadHeadStart,tHeadData,tDataHeadPrefix,tDataDataStart,tDataData};
const int delayTable[4][2] =
{
	{2168	,2168},
	{735   	,667},
	{1710 	,1710},
	{855	,855}
};
#define PILOT_P  0
#define START_P 1
#define ONE_P    2
#define ZERO_P  3

struct  TapeState
{
	//~ enum InState{} = { InHeader,InData
	enum eState  state;
	int  bBit;
	int32_t  prevT;
	uint32_t prevT_frame;
	int64_t snext;
	int64_t integrateTime;
	int  brPause;

	int coundReps;
	int bytesRemain;

	uint8_t fileBuffer[FILE_BUFFER_SIZE];
	uint8_t lastDataByte;
	int     fileBufferPos;
	int     lastRealSize;
	int     bTape;


}TapeState;


void tapeStart()
{
	TapeState.bTape = 1;
}
void tapeStop()
{
	TapeState.bTape = 0;
}
int getTapeState()
{
	return TapeState.bTape;
}
void TapeState_constructor()
{
	TapeState.state = tUndef;
	TapeState.coundReps = 0;
	TapeState.bytesRemain = 0;
	TapeState.fileBufferPos = 0;
	TapeState.lastRealSize = 0;
	TapeState.lastDataByte = 0;
	TapeState.bBit = 1;
	TapeState.prevT = 0;
	TapeState.prevT_frame = 0;
	TapeState.snext = 0;
	TapeState.integrateTime = 0;
	TapeState.brPause = 0;
	tapeStop();
}
void tape_close()
{
	if(ifs_tape)
	{
		fclose(ifs_tape);
		ifs_tape = 0;
	}
	TapeState.bTape = 0;
}
int tape_open(char* fileName)
{
	ifs_tape = fopen(fileName,"rb");
	if(ifs_tape)
	{
		TapeState_constructor();
		return 1;
	}
	return 0;
}

int getDataByte(uint8_t* bt)
{
	if(TapeState.fileBufferPos>=TapeState.lastRealSize)
	{
		TapeState.lastRealSize = 0;
		TapeState.fileBufferPos= 0;
		if(ifs_tape)
		{
			size_t bytes = 0;
			//f_read(ifs_tape,(char*)TapeState.fileBuffer,FILE_BUFFER_SIZE,&bytes);
			bytes = fread((char*)TapeState.fileBuffer,1,FILE_BUFFER_SIZE,ifs_tape);
			//ifs_tape->read((char*)fileBuffer,FILE_BUFFER_SIZE);
			TapeState.lastRealSize = bytes;
			//~ PRINT(lastRealSize);
		}
		if(!TapeState.lastRealSize)
		{
			if(ifs_tape)
			{
				tape_close();
			}
			*bt = 0;
			return 1;
		}

	}
	*bt = TapeState.fileBuffer[TapeState.fileBufferPos];
	TapeState.fileBufferPos++;
	return 0;
}

int32_t getNextDelay()
{
	int Bit       =   1;
	int tDelay  =  0;
	switch(TapeState.state)
	{
		case  tNoFile:
				Bit = 3;
				break;
		case  tUndef:
			{
				//~ integrateTime = 0;
				//~ snext = 0;
				//~ prevT = tstates ;
				if(!TapeState.brPause)
				{
					uint8_t bt0;
					uint8_t bt1;
					int res  =  getDataByte(&bt0);
					res      += getDataByte(&bt1);
					TapeState.bytesRemain = ((int)bt1)*256+bt0;
					printf(" bytesRemain = %d\r\n",TapeState.bytesRemain);
					if(TapeState.bytesRemain==19)
					{
						TapeState.state = tHeadHeadPrefix;
						TapeState.coundReps = 8063*2;
						Bit = 1;
						tDelay = 1000;
					}
					else if(TapeState.bytesRemain==20)
					{
						TapeState.bTape = 0;
						TapeState.brPause = 1;
						tDelay = 1000;
						Bit = 3;
					}
					else if(TapeState.bytesRemain>0)
					{
						TapeState.state = tDataHeadPrefix;
						TapeState.coundReps = 3233*2;
						tDelay = 1000;
						Bit = 1;
					}
					else
					{
						TapeState.state = tNoFile;
						Bit = 3;
					}
				}
				else
				{
					TapeState.brPause = 0;
					TapeState.bytesRemain = 20;
					TapeState.state = tDataHeadPrefix;
					TapeState.coundReps = 3233*2;
					Bit = 2;
				}
			}
			break;

		case  tHeadHeadPrefix:
			Bit      =  1-(TapeState.coundReps&1);
			tDelay =  delayTable[PILOT_P][Bit];
			TapeState.coundReps--;
			//~ PRINT(coundReps);
			if(TapeState.coundReps<=0)
			{
				TapeState.state = tHeadHeadStart;
				TapeState.coundReps = 2;
			}
			break;
		case  tHeadHeadStart:
			Bit      =  1-(TapeState.coundReps&1);
			tDelay =  delayTable[START_P][Bit];
			printf("tDelay %d\r\n",tDelay);
			TapeState.coundReps--;
			if(TapeState.coundReps<=0)
			{
				TapeState.state = tHeadData;
			}
			break;

		case  tHeadData:
			{
				if(TapeState.coundReps<=0)
				{
					TapeState.bytesRemain--;
					//~ PRINT(bytesRemain);
					if(TapeState.bytesRemain>=0)
					{
						getDataByte(&TapeState.lastDataByte);
						TapeState.coundReps = 16;
						//~ Bit = 2;
					}
					else
					{
						Bit = 1;
						tDelay = 1000;
						TapeState.state = tUndef;
					}
				}
				 // enter with 16
				if(TapeState.coundReps>0)
				{
					Bit      =  1-(TapeState.coundReps&1);
					int bitShift = (TapeState.coundReps-1)/2;
					int lastBit   = (TapeState.lastDataByte>>bitShift)&1;
					tDelay =  delayTable[lastBit?ONE_P:ZERO_P][Bit];
					TapeState.coundReps--;
				}
			}
			break;
		case  tDataHeadPrefix:
			Bit      =  1-(TapeState.coundReps&1);
			tDelay =  delayTable[PILOT_P][Bit];
			TapeState.coundReps--;
			if(TapeState.coundReps<=0)
			{
				TapeState.state = tDataDataStart;
				TapeState.coundReps = 2;
			}
			break;
		case  tDataDataStart:
			Bit      =  1-(TapeState.coundReps&1);
			tDelay =  delayTable[START_P][Bit];
			TapeState.coundReps--;
			if(TapeState.coundReps<=0)
			{
				TapeState.state = tDataData;
			}
			break;
		case  tDataData:
			{
				if(TapeState.coundReps<=0)
				{
					TapeState.bytesRemain--;
					//~ PRINT(bytesRemain);
					if(TapeState.bytesRemain>=0)
					{
						getDataByte(&TapeState.lastDataByte);
						TapeState.coundReps = 16;
						//~ Bit = 2;
					}
					else
					{
						Bit = 1;
						tDelay = 1000;
						TapeState.state = tUndef;
					}
				}
				 // enter with 16
				if(TapeState.coundReps>0)
				{
					Bit      =  1-(TapeState.coundReps&1);
					int bitShift = (TapeState.coundReps-1)/2;
					int lastBit   = (TapeState.lastDataByte>>bitShift)&1;
					tDelay =  delayTable[lastBit?ONE_P:ZERO_P][Bit];
					TapeState.coundReps--;
				}
			}
			break;
	}
	return (Bit<<16) | tDelay;
}


int 	 getT69888();
uint32_t getFrameCounter();

void tapeSetTime()
{
	TapeState.prevT = tstates;
	TapeState.prevT_frame = getFrameCounter();
}
int  tapeBit()
{
	if(!TapeState.bTape)
	{
		TapeState.integrateTime = 0;
		TapeState.snext = 0;
		TapeState.prevT = tstates;
		TapeState.prevT_frame = getFrameCounter();
		return 1;
	}

	int64_t delta = ((tstates - TapeState.prevT)+(getFrameCounter()-TapeState.prevT_frame)*getT69888())&(int64_t)-2;

	TapeState.prevT = tstates;
	TapeState.prevT_frame = getFrameCounter();
	//while(delta<0) delta+=getT69888();
	TapeState.integrateTime+= delta;
	int32_t     sinc = 1;
	//~ bBit           = 1;
	int rbBit = 0;
	while(TapeState.integrateTime>=TapeState.snext && (TapeState.bBit != 3)&&(sinc!=0))
	{
		sinc   = getNextDelay();
		rbBit =   (sinc>>16);
		while(rbBit==2)
		{
			sinc   = getNextDelay();
			rbBit =  (sinc>>16);
		}
		if(rbBit==3)
		{
			if(!ifs_tape)
			{
				TapeState.bTape = 0;
			}
			sinc = 0;
		}
		sinc&=0xffff;
		TapeState.snext += sinc;//states.front();states.pop();
		TapeState.bBit = rbBit&1;
	}
static int oldBit = 0;
	if(oldBit!=TapeState.bBit)
	{
		soundEvents(TapeState.bBit);
		oldBit=TapeState.bBit;
	}
	//~ PRINT(bBit);
	return TapeState.bBit;
}
