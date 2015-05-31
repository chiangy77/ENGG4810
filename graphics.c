#include "graphics.h"
#include "fonts.h"

extern void setArea(u_char xStart, u_char yStart, u_char xEnd, u_char yEnd);
extern void writeData(u_char data);
extern u_char getScreenWidth();
extern u_char getScreenHeight();

u_char colorLowByte = 0;
u_char colorHighByte = 0;
u_char bgColorLowByte = 0;
u_char bgColorHighByte = 0;

void setColor(u_int color) {
	colorLowByte = color;
	colorHighByte = color >> 8;
}

void setBackgroundColor(u_int color) {
	bgColorLowByte = color;
	bgColorHighByte = color >> 8;
}

void clearScreen(u_char blackWhite) {
	u_char w = getScreenWidth();
	u_char h = getScreenHeight();
	setArea(0, 0, w - 1, h - 1);
	setBackgroundColor(blackWhite ? 0x0000 : 0xFFFF);

	while (h != 0) {
		while (w != 0) {
			writeData(bgColorHighByte);
			writeData(bgColorLowByte);
			w--;
		}
		w = getScreenWidth();
		h--;
	}
}

void drawPixel(u_char x, u_char y) {
	setArea(x, y, x, y);
	writeData(colorHighByte);
	writeData(colorLowByte);
}

// Draw String - type: 0=Sm, 1=Md, 2=Lg
void drawString(u_char x, u_char y, char type, char *string) {
	u_char xs = x;

	switch (type) {
	case FONT_SM:
		while (*string) {
			drawCharSm(xs, y, *string++);
			xs += 6;
		}
		break;
	case FONT_MD:
		while (*string) {
			drawCharMd(xs, y, *string++);
			xs += 8;
		}
		break;
	case FONT_LG:
		while (*string) {
			drawCharLg(xs, y, *string++);
			xs += 12;
		}
		break;
	}
}

// 5x7 font
void drawCharSm(u_char x, u_char y, char c) {
	u_char col = 0;
	u_char row = 0;
	u_char bit = 0x01;
	u_char oc = c - 0x20;
	while (row < 8) {
		while (col < 5) {
			if (font_5x7[oc][col] & bit)
				drawPixel(x + col, y + row);
			col++;
		}
		col = 0;
		bit <<= 1;
		row++;
	}
}

// 11x16 font
void drawCharLg(u_char x, u_char y, char c) {
	u_char col = 0;
	u_char row = 0;
	u_int bit = 0x0001;
	u_char oc = c - 0x20;
	while (row < 16) {
		while (col < 11) {
			if (font_11x16[oc][col] & bit)
				drawPixel(x + col, y + row);
			col++;
		}
		col = 0;
		bit <<= 1;
		row++;
	}
}

// 8x12 font
void drawCharMd(u_char x, u_char y, char c) {
	u_char col = 0;
	u_char row = 0;
	u_char bit = 0x80;
	u_char oc = c - 0x20;
	while (row < 12) {
		while (col < 8) {
			if (font_8x12[oc][row] & bit)
				drawPixel(x + col, y + row);
			bit >>= 1;
			col++;
		}
		bit = 0x80;
		col = 0;
		row++;
	}
}


//Shapes

void drawLine(u_char xStart, u_char yStart, u_char xEnd, u_char yEnd) {

	u_char x0, x1, y0, y1;
	u_char d = 0;

// handle direction
	if (yStart > yEnd) {
		y0 = yEnd;
		y1 = yStart;
	} else {
		y1 = yEnd;
		y0 = yStart;
	}

	if (xStart > xEnd) {
		x0 = xEnd;
		x1 = xStart;
	} else {
		x1 = xEnd;
		x0 = xStart;
	}

// check if horizontal
	if (y0 == y1) {
		d = x1 - x0 + 1;
		setArea(x0, y0, x1, y1);
		while (d-- > 0) {
			writeData(colorHighByte);
			writeData(colorLowByte);
		}

	} else if (x0 == x1) { // check if vertical
		d = y1 - y0 + 1;
		setArea(x0, y0, x1, y1);
		while (d-- > 0) {
			writeData(colorHighByte);
			writeData(colorLowByte);
		}

	} else { // angled
		char dx, dy;
		int sx, sy;

		if (xStart < xEnd) {
			sx = 1;
			dx = xEnd - xStart;
		} else {
			sx = -1;
			dx = xStart - xEnd;
		}

		if (yStart < yEnd) {
			sy = 1;
			dy = yEnd - yStart;
		} else {
			sy = -1;
			dy = yStart - yEnd;
		}

		int e1 = dx - dy;
		int e2;

		while (1) {
			drawPixel(xStart, yStart);
			if (xStart == xEnd && yStart == yEnd)
				break;
			e2 = 2 * e1;
			if (e2 > -dy) {
				e1 = e1 - dy;
				xStart = xStart + sx;
			}
			if (e2 < dx) {
				e1 = e1 + dx;
				yStart = yStart + sy;
			}
		}
	}
}

void drawRect(u_char xStart, u_char yStart, u_char xEnd, u_char yEnd) {

	drawLine(xStart, yStart, xEnd, yStart);
	drawLine(xStart, yEnd, xEnd, yEnd);
	drawLine(xStart, yStart, xStart, yEnd);
	drawLine(xEnd, yStart, xEnd, yEnd);
}

void drawCircle(u_char x, u_char y, u_char radius) {
	int dx = radius;
	int dy = 0;
	int xChange = 1 - 2 * radius;
	int yChange = 1;
	int radiusError = 0;
	while (dx >= dy) {
		drawPixel(x + dx, y + dy);
		drawPixel(x - dx, y + dy);
		drawPixel(x - dx, y - dy);
		drawPixel(x + dx, y - dy);
		drawPixel(x + dy, y + dx);
		drawPixel(x - dy, y + dx);
		drawPixel(x - dy, y - dx);
		drawPixel(x + dy, y - dx);
		dy++;
		radiusError += yChange;
		yChange += 2;
		if (2 * radiusError + xChange > 0) {
			dx--;
			radiusError += xChange;
			xChange += 2;
		}
	}
}

void drawHalfCircle(u_char x, u_char y, u_char radius) {
	int dx = radius;
	int dy = 0;
	int xChange = 1 - 2 * radius;
	int yChange = 1;
	int radiusError = 0;
	while (dx >= dy) {
		drawPixel(x - dx, y - dy);
		drawPixel(x + dx, y - dy);
		drawPixel(x - dy, y - dx);
		drawPixel(x + dy, y - dx);
		dy++;
		radiusError += yChange;
		yChange += 2;
		if (2 * radiusError + xChange > 0) {
			dx--;
			radiusError += xChange;
			xChange += 2;
		}
	}
}

void fillRect(u_char xStart, u_char yStart, u_char xEnd, u_char yEnd) {
	setArea(xStart, yStart, xEnd, yEnd);
	u_int total = (xEnd - xStart + 1) * (yEnd - yStart + 1);
	u_int c = 0;
	while (c < total) {
		writeData(colorHighByte);
		writeData(colorLowByte);
		c++;
	}
}

void fillCircle(u_char x, u_char y, u_char radius) {
	int dx = radius;
	int dy = 0;
	int xChange = 1 - 2 * radius;
	int yChange = 1;
	int radiusError = 0;
	while (dx >= dy) {
		drawLine(x + dy, y + dx, x - dy, y + dx);
		drawLine(x - dy, y - dx, x + dy, y - dx);
		drawLine(x - dx, y + dy, x + dx, y + dy);
		drawLine(x - dx, y - dy, x + dx, y - dy);
		dy++;
		radiusError += yChange;
		yChange += 2;
		if (2 * radiusError + xChange > 0) {
			dx--;
			radiusError += xChange;
			xChange += 2;
		}
	}
}



