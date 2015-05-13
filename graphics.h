#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "typedefs.h"

// font sizes
#define FONT_SM		0
#define FONT_MD		1
#define FONT_LG		2

void setColor(u_int color);
void setBackgroundColor(u_int color);
void clearScreen(u_char blackWhite);
void drawPixel(u_char x, u_char y);
void drawString(u_char x, u_char y, char type, char *string);
void drawCharLg(u_char x, u_char y, char c);
void drawCharMd(u_char x, u_char y, char c);
void drawCharSm(u_char x, u_char y, char c);
void drawLine(u_char xStart, u_char yStart, u_char xEnd, u_char yEnd);
void drawRect(u_char xStart, u_char yStart, u_char xEnd, u_char yEnd);
void drawCircle(u_char x, u_char y, u_char radius);
void drawHalfCircle(u_char x, u_char y, u_char radius);
void fillRect(u_char xStart, u_char yStart, u_char xEnd, u_char yEnd);
void fillCircle(u_char x, u_char y, u_char radius);


#endif /* GRAPHICS_H_ */

