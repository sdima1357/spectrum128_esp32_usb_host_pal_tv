#define SW (256+32)
#define SH  240
#define BORDER_W  0
uint16_t LCD_getWidth();
uint16_t LCD_getHeight();
void clearFullScreen();
typedef uint8_t colorType;
void  LCD_Draw_Text(const char* Text, int16_t X, int16_t Y, colorType Colour, uint16_t Size, colorType Background_Colour);
void LCD_Draw_Char(char Character, int16_t X, int16_t Y, colorType Colour, uint16_t SizeX, colorType Background_Colour);
void LCD_fillRect(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, colorType color);
void LCD_fillRectP(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, colorType *color);

#define MCOLOR(Ra,Ga,Ba)  (((Ra)<<4)|((Ga)<<2)|(Ba))

#define BLACK          MCOLOR(0,0,0)      /*   0,   0,   0 */
#define NAVY            MCOLOR(0,0,2)      /*   0,   0, 128 */
#define DGREEN       MCOLOR(0,2,0)      /*   0, 128,   0 */
#define DCYAN          MCOLOR(0,2,2)     /*   0, 128, 128 */
#define MAROON       MCOLOR(2,0,0)     /* 128,   0,   0 */
#define PURPLE         MCOLOR(2,0,2)      /* 128,   0, 128 */
#define OLIVE           MCOLOR(2,2,0)      /* 128, 128,   0 */
#define LGRAY           MCOLOR(2,2,2)      /* 192, 192, 192 */
#define DGRAY          MCOLOR(1,1,1)      /* 128, 128, 128 */
#define BLUE            MCOLOR(0,0,3)      /*   0,   0, 255 */
#define GREEN          MCOLOR(0,3,0)    /*   0, 255,   0 */
#define CYAN            MCOLOR(0,3,3)      /*   0, 255, 255 */
#define RED              MCOLOR(3,0,0)     /* 255,   0,   0 */
#define MAGENTA      MCOLOR(3,0,3)     /* 255,   0, 255 */
#define YELLOW        MCOLOR(3,3,0)      /* 255, 255,   0 */
#define WHITE           MCOLOR(3,3,3)      /* 255, 255, 255 */
#define ORANGE        MCOLOR(3,2,0)     /* 255, 165,   0 */
#define BGROUND     MCOLOR(0,0,0)

#include "fonts.h"

extern sFONT* FONT_CURR;

#define CHAR_WIDTH (FONT_CURR->Width)
#define CHAR_HEIGHT (FONT_CURR->Height)
