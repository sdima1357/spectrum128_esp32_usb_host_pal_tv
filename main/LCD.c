#include <stdint.h>
#include "lcd.h"

uint16_t LCD_getWidth() {
    return SW;
}

uint16_t LCD_getHeight() {
    return SH;
}
#define MAX(X,Y) ((X>Y)?X:Y)
#define MIN(X,Y) ((X<Y)?X:Y)
//~ /*
uint8_t makeColor(int R,int G,int B) //332
{
	//return MCOLOR(R,G,B);
	return ((R>>6)<<4)|((G>>6)<<2)|(B>>6);
}
//~ */
colorType colorTableA[] =
{
		NAVY,
		YELLOW,
		WHITE,
		MAGENTA,
		RED,
		CYAN,
		GREEN,
		BLUE,
		OLIVE,
		PURPLE,
		MAROON,
		NAVY,
		DGREEN
};

inline int clamp(int val,int minval,int maxval)
{
	return MAX(MIN(maxval,val),minval);
}

extern uint8_t   screen[SH][SW];

void LCD_fillRect(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, colorType color)
{
	color&=0x3f;
	x1 = clamp(x1,0,SW);
	int x2 = clamp(x1+w,0,SW);
	y1 = clamp(y1,0,SH);
	int y2 = clamp(y1+h,0,SH);
	//printf("%d,%d %d,%d\n",x1,y1,x2,y2);
	for(int y = y1; y<y2;y++)
	{
		for(int x = x1; x<x2;x++)
		{
			screen[y][x] = color;
		}
	}
}
void LCD_fillRectP(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, colorType *color)
{
	x1 = clamp(x1,0,SW);
	int x2 = clamp(x1+w,0,SW);
	y1 = clamp(y1,0,SH);
	int y2 = clamp(y1+h,0,SH);
	//printf("%d,%d %d,%d\n",x1,y1,x2,y2);
	for(int y = y1; y<y2;y++)
	{
		for(int x = x1; x<x2;x++)
		{
			screen[y][x] = (*color++)&0x3f;
		}
	}
}

inline void LCD_DrawHLine(uint16_t x1,uint16_t y1,uint16_t length,colorType color)
{
	LCD_fillRect(x1,y1,length,1,color);
}
inline void LCD_DrawVLine(uint16_t x1,uint16_t y1,uint16_t length,colorType color)
{
	LCD_fillRect(x1,y1,1,length,color);
}
inline void LCD_DrawPixel(uint16_t x1, uint16_t y1,colorType color)
{
	LCD_fillRect(x1, y1, 1, 1,color);
}
#define ABS(X)  ((X) > 0 ? (X) : -(X))
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,colorType color)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
  curpixel = 0;

  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */

  if (x2 >= x1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (y2 >= y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         /* There are more y-values than x-values */
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    LCD_DrawPixel(x, y , color);  /* Draw the current pixel */
    num += numadd;                            /* Increase the numerator by the top of the fraction */
    if (num >= den)                           /* Check if numerator >= denominator */
    {
      num -= den;                             /* Calculate the new numerator value */
      x += xinc1;                             /* Change the x as appropriate */
      y += yinc1;                             /* Change the y as appropriate */
    }
    x += xinc2;                               /* Change the x as appropriate */
    y += yinc2;                               /* Change the y as appropriate */
  }
}

void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius,colorType color)
{
   if(Radius==0)
   {
	   LCD_DrawPixel(Xpos,Ypos,color);
	   return;
   }

  int32_t  D;       /* Decision Variable */
  uint32_t  CurX;   /* Current X Value */
  uint32_t  CurY;   /* Current Y Value */

  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;

  while (CurX <= CurY)
  {
    LCD_DrawPixel((Xpos + CurX), (Ypos - CurY), color);

    LCD_DrawPixel((Xpos - CurX), (Ypos - CurY), color);

    LCD_DrawPixel((Xpos + CurY), (Ypos - CurX), color);

    LCD_DrawPixel((Xpos - CurY), (Ypos - CurX), color);

    LCD_DrawPixel((Xpos + CurX), (Ypos + CurY), color);

    LCD_DrawPixel((Xpos - CurX), (Ypos + CurY), color);

    LCD_DrawPixel((Xpos + CurY), (Ypos + CurX), color);

    LCD_DrawPixel((Xpos - CurY), (Ypos + CurX), color);


    if (D < 0)
    {
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}
void LCD_DrawCircleFull(uint16_t Xpos, uint16_t Ypos, uint16_t Radius,colorType color)
{
   if(Radius==0)
   {
	   LCD_DrawPixel(Xpos,Ypos,color);
	   return;
   }

  int32_t  D;       /* Decision Variable */
  uint32_t  CurX;   /* Current X Value */
  uint32_t  CurY;   /* Current Y Value */

  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;

  while (CurX <= CurY)
  {
//    LCD_DrawPixel((Xpos + CurX), (Ypos - CurY), color);
//    LCD_DrawPixel((Xpos - CurX), (Ypos - CurY), color);
    LCD_DrawHLine(Xpos - CurX,Ypos - CurY,CurX*2,color);

//    LCD_DrawPixel((Xpos + CurY), (Ypos - CurX), color);
//    LCD_DrawPixel((Xpos - CurY), (Ypos - CurX), color);
    LCD_DrawHLine(Xpos - CurY,Ypos - CurX,CurY*2,color);


//    LCD_DrawPixel((Xpos + CurX), (Ypos + CurY), color);
//    LCD_DrawPixel((Xpos - CurX), (Ypos + CurY), color);
    LCD_DrawHLine(Xpos - CurX,Ypos + CurY,CurX*2,color);


//    LCD_DrawPixel((Xpos + CurY), (Ypos + CurX), color);
//    LCD_DrawPixel((Xpos - CurY), (Ypos + CurX), color);
    LCD_DrawHLine(Xpos - CurY,Ypos + CurX,CurY*2,color);


    if (D < 0)
    {
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}

void LCD_DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius,colorType color)
{
  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float K = 0, rad1 = 0, rad2 = 0;

  rad1 = XRadius;
  rad2 = YRadius;

  K = (float)(rad2/rad1);

  do {
	  uint16_t dt = (uint16_t)(x/K);
    LCD_DrawPixel(Xpos-dt, (Ypos+y), color);
    LCD_DrawPixel(Xpos+dt, (Ypos+y), color);
    LCD_DrawPixel(Xpos+dt, (Ypos-y), color);
    LCD_DrawPixel(Xpos-dt, (Ypos-y), color);

    e2 = err;
    if (e2 <= x) {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;
  }
  while (y <= 0);
}
void LCD_DrawEllipseFull(int Xpos, int Ypos, int XRadius, int YRadius,colorType color)
{
  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float K = 0, rad1 = 0, rad2 = 0;

  rad1 = XRadius;
  rad2 = YRadius; 

  K = (float)(rad2/rad1);

  do {
	  uint16_t dt = (uint16_t)(x/K);
	  LCD_DrawHLine(Xpos-dt, (Ypos+y),dt*2, color);
	  LCD_DrawHLine(Xpos-dt, (Ypos-y),dt*2, color);

    e2 = err;
    if (e2 <= x) {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;
  }
  while (y <= 0);
}

#include "fonts.h"
sFONT* FONT_CURR =  &Font9x16;
#define CHAR_WIDTH (FONT_CURR->Width)
#define CHAR_HEIGHT (FONT_CURR->Height)


void LCD_Draw_Char2(char Character, int16_t X, int16_t Y, colorType Colour, uint16_t SizeX,uint16_t SizeY, colorType Background_Colour)
{
	//~ flagReinit = 1;
	uint8_t 	function_char;
    int16_t 	i,j;

	function_char = Character;

	// Draw pixels
	LCD_fillRect(X, Y, CHAR_WIDTH*SizeX, CHAR_HEIGHT*SizeY, Background_Colour);

    if (function_char <= 0x20)
	{

	}
	else
	{
		function_char -= 0x20;
		int rw = (CHAR_WIDTH+7)/8;
		for (j=0; j<FONT_CURR->Height; j++)
		{
			uint8_t* pnt =  FONT_CURR->table+j*rw+function_char*FONT_CURR->Height*rw;
			for (i=0; i<FONT_CURR->Width; i++)
			{
				uint8_t bt = pnt[i/8];
				if(bt&(1<<(7-(i&7))))
				{
					LCD_fillRect(X+(i*SizeX), Y+(j*SizeY), SizeX,SizeY, Colour);
				}
			}
		}
	}
}
void LCD_Draw_Char(char Character, int16_t X, int16_t Y, colorType Colour, uint16_t SizeX, colorType Background_Colour)
{
	LCD_Draw_Char2(Character,X,Y,Colour,SizeX,SizeX,Background_Colour);
}

void  LCD_Draw_Text(const char* Text, int16_t X, int16_t Y, colorType Colour, uint16_t Size, colorType Background_Colour)
{
    while (*Text) {
        LCD_Draw_Char2(*Text, X, Y, Colour, Size, Size,Background_Colour);
        X += CHAR_WIDTH*Size;
	Text++;
    }
}

void ClrScr()
{
	LCD_fillRect(0,0,LCD_getWidth(),LCD_getHeight(),BGROUND);
}
void convertColors(){}
uint32_t  HAL_GetTick();
void HAL_Delay(int ms);

void clearFullScreen()
{
	LCD_fillRect(0, 0, LCD_getWidth(), LCD_getHeight(), BLACK);
}

#include <stdio.h>
#include <stdlib.h>

void dTest()
{
	  HAL_Delay(2000);
	  if(1){
			uint32_t trt = HAL_GetTick();
			int k;
			//setBK_imp(99);

			for(k=0;k<1000;k++)
			{
				LCD_fillRect(0, 0, LCD_getWidth(), LCD_getHeight(), colorTableA[rand()&0xf]);
				//HAL_Delay(1000);
			}
			trt = (HAL_GetTick()-trt);
			printf("Rect Full %d ms\n",trt);
			HAL_Delay(1000);
			LCD_fillRect(0, 0, LCD_getWidth(), LCD_getHeight(), BLACK);
	  }
	  if(1){
			uint32_t trt = HAL_GetTick();
			int k;
			//setBK_imp(99);

			for(k=0;k<1000;k++)
			{
				int IW = rand()%(LCD_getWidth()/2);
				int IH = rand()%(LCD_getHeight()/2);
				int xPos =rand()%(LCD_getWidth()-IW);
				int yPos =rand()%(LCD_getHeight()-IH);

				LCD_fillRect(xPos, yPos, IW, IH,rand());
				convertColors();
				//HAL_GPIO_TogglePin(BOARD_LED_GPIO_Port,BOARD_LED_Pin);
			}
			trt = (HAL_GetTick()-trt);
			printf("Rect Full %d ms\n",trt);
			HAL_Delay(1000);
			LCD_fillRect(0, 0, LCD_getWidth(), LCD_getHeight(), BLACK);
	  }
	  if(1){
			uint32_t trt = HAL_GetTick();
			int k;
			//setBK_imp(99);

			for(k=0;k<1000;k++)
			{
				int IW = rand()%(LCD_getWidth()/2);
				int IH = rand()%(LCD_getHeight()/2);
				int xPos =rand()%(LCD_getWidth()-IW);
				int yPos =rand()%(LCD_getHeight()-IH);

				LCD_fillRect(xPos, yPos, IW, IH,rand());
				convertColors();
				//HAL_GPIO_TogglePin(BOARD_LED_GPIO_Port,BOARD_LED_Pin);
			}
			trt = (HAL_GetTick()-trt);
			printf("Rect %d ms\n",trt);
			HAL_Delay(1000);
			LCD_fillRect(0, 0, LCD_getWidth(), LCD_getHeight(), BLACK);
	  }
	  if(0){
			uint32_t trt = HAL_GetTick();
			int k;
			//setBK_imp(99);

			for(k=0;k<1000;k++)
			{
				int xPos =rand()%(LCD_getWidth());
				int yPos =rand()%(LCD_getHeight());
				int xPos1 =rand()%(LCD_getWidth());
				int yPos1 =rand()%(LCD_getHeight());

				LCD_DrawLine(xPos, yPos,xPos1,yPos1,rand());
				convertColors();
				//HAL_GPIO_TogglePin(BOARD_LED_GPIO_Port,BOARD_LED_Pin);
			}
			trt = (HAL_GetTick()-trt);
			printf("Lines %d ms\n",trt);
			HAL_Delay(1000);
			LCD_fillRect(0, 0, LCD_getWidth(), LCD_getHeight(), BLACK);
	  }
	  if(1){
			uint32_t trt = HAL_GetTick();
			int k;
			//setBK_imp(99);

			for(k=0;k<1000;k++)
			{
				int R = (rand()%(LCD_getHeight()/4));
				int xPos =rand()%(LCD_getWidth()-R*2);
				int yPos =rand()%(LCD_getHeight()-R*2);

				LCD_DrawCircle(xPos+R, yPos+R, R ,rand());
				convertColors();
				//HAL_GPIO_TogglePin(BOARD_LED_GPIO_Port,BOARD_LED_Pin);
			}
			trt = (HAL_GetTick()-trt);
			printf("Circle %d ms\n",trt);
			HAL_Delay(1000);
			LCD_fillRect(0, 0, LCD_getWidth(), LCD_getHeight(), BLACK);
	  }
	  if(1){
			uint32_t trt = HAL_GetTick();
			int k;
			//setBK_imp(99);

			for(k=0;k<1000;k++)
			{
				int R = (rand()%(LCD_getHeight()/4));
				int xPos =rand()%(LCD_getWidth()-R*2);
				int yPos =rand()%(LCD_getHeight()-R*2);

				LCD_DrawCircleFull(xPos+R, yPos+R, R ,rand());
				convertColors();
				//HAL_GPIO_TogglePin(BOARD_LED_GPIO_Port,BOARD_LED_Pin);
			}
			trt = (HAL_GetTick()-trt);
			printf("Circle Full %d ms\n",trt);
			HAL_Delay(1000);
			LCD_fillRect(0, 0, LCD_getWidth(), LCD_getHeight(), BLACK);
	  }
	  if(0){
			uint32_t trt = HAL_GetTick();
			int k;
			//setBK_imp(99);

			for(k=0;k<1000;k++)
			{
				int R = (rand()%(LCD_getHeight()/4));
				int xPos =rand()%(LCD_getWidth()-R*2);
				int yPos =rand()%(LCD_getHeight()-R*2);

				LCD_DrawEllipse(xPos+R, yPos+R, R ,R+1,rand());
				convertColors();
				//HAL_GPIO_TogglePin(BOARD_LED_GPIO_Port,BOARD_LED_Pin);
			}
			trt = (HAL_GetTick()-trt);
			printf("Ellipse %d ms\n",trt);
			HAL_Delay(1000);
			LCD_fillRect(0, 0, LCD_getWidth(), LCD_getHeight(), BLACK);
	  }
	  if(1)
	  {
			uint32_t trt = HAL_GetTick();
			int k;
			//setBK_imp(99);

			for(k=0;k<1000;k++)
			{
				int R = (rand()%(LCD_getHeight()/4));
				int xPos =rand()%(LCD_getWidth()-R*2);
				int yPos =rand()%(LCD_getHeight()-R*2);

				LCD_DrawEllipseFull(xPos+R, yPos+R, R ,R+1,rand());
				convertColors();
				//HAL_GPIO_TogglePin(BOARD_LED_GPIO_Port,BOARD_LED_Pin);
			}
			trt = (HAL_GetTick()-trt);
			printf("Ellipse Full %d ms\n",trt);
			HAL_Delay(1000);
			LCD_fillRect(0, 0, LCD_getWidth(), LCD_getHeight(), BLUE);
	  }
	  {
			uint32_t trt = HAL_GetTick();
			int k;
			//setBK_imp(99);

			for(k=0;k<1000;k++)
			{
				int IW = rand()%(LCD_getWidth()/2);
				int IH = rand()%(LCD_getHeight()/2);
				int xPos =rand()%(LCD_getWidth()-IW);
				int yPos =rand()%(LCD_getHeight()-IH);

				LCD_fillRect(xPos, yPos, IW, IH,rand());
				convertColors();
				//HAL_GPIO_TogglePin(BOARD_LED_GPIO_Port,BOARD_LED_Pin);
			}
			trt = (HAL_GetTick()-trt);
			printf("Rect Full %d ms\n",trt);
			HAL_Delay(1000);
			LCD_fillRect(0, 0, LCD_getWidth(), LCD_getHeight(), RED);
			HAL_Delay(1000);
	  }
	  {
		  printf("CHAR_WIDTH=%d CHAR_HEIGHT=%d\n",CHAR_WIDTH,CHAR_HEIGHT);
		  LCD_fillRect(0, 0, LCD_getWidth(), LCD_getHeight(), BLACK);
		  for(int y=0;y<SH-CHAR_HEIGHT+1;y+=CHAR_HEIGHT)
		  {
			  for(int x=0;x<SW-CHAR_WIDTH+1;x+=CHAR_WIDTH)
			  {
				int charp = (rand()%('z'-'A'+1)) +'A'; 
				LCD_Draw_Char2(charp,x,y,colorTableA[rand()%13],1,1,colorTableA[rand()%13]);
			  }
		  }
	  }

}

