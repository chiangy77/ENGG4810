#include "lcd.h"
#include "tiva.h"
#include "config.h"

u_char _orientation = 0;

u_char getScreenWidth() {
	if (_orientation == 0 || _orientation == 2)
		return SHORT_EDGE_PIXELS;
	else
		return LONG_EDGE_PIXELS;
}

u_char getScreenHeight() {
	if (_orientation == 0 || _orientation == 2)
		return LONG_EDGE_PIXELS;
	else
		return SHORT_EDGE_PIXELS;
}

void setArea(u_char xStart, u_char yStart, u_char xEnd, u_char yEnd) {

#ifdef ILI9225

if(_orientation == 0 || _orientation == 2) {

	writeCommand(ILIGRAMHEA);
	writeData(0);
	writeData(xEnd);
	writeCommand(ILIGRAMHSA);
	writeData(0);
	writeData(xStart);
	writeCommand(ILIGRAMADDRX);
	writeData(0);
	writeData(xStart);

	writeCommand(ILIGRAMVEA);
	writeData(0);
	writeData(yEnd);
	writeCommand(ILIGRAMVSA);
	writeData(0);
	writeData(yStart);
	writeCommand(ILIGRAMADDRY);
	writeData(0);
	writeData(yStart);

} else {

	writeCommand(ILIGRAMHEA);
	writeData(0);
	writeData(yEnd);
	writeCommand(ILIGRAMHSA);
	writeData(0);
	writeData(yStart);
	writeCommand(ILIGRAMADDRX);
	writeData(0);
	writeData(yStart);

	writeCommand(ILIGRAMVEA);
	writeData(0);
	writeData(xEnd);
	writeCommand(ILIGRAMVSA);
	writeData(0);
	writeData(xStart);
	writeCommand(ILIGRAMADDRY);
	writeData(0);
	writeData(xStart);
}

	writeCommand(ILIWRDATA);

#else
	writeCommand(CASETP);

	writeData(0);
	writeData(xStart);

	writeData(0);
	writeData(xEnd);

	writeCommand(PASETP);

	writeData(0);
	writeData(yStart);

	writeData(0);
	writeData(yEnd);

	writeCommand(RAMWRP);
#endif //ILI switch
	// data to follow
}

////////////////////////////////////
// gamma, lut, and other inits
////////////////////////////////////


#ifdef ST7735

void gammaAdjustmentST7735() {
	const u_char gmctrp1[] = {0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d,
		0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10};
	const u_char gmctrn1[] = {0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
		0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10};
// gamma correction is needed for accurate color but is not necessary.
	u_char c = 0;
	writeCommand(ST7735_GMCTRP1);// gamma adjustment (+ polarity)
	while (c < 16) {
		writeData(gmctrp1[c]);
		c++;
	}
	c = 0;
	writeCommand(ST7735_GMCTRN1); // gamma adjustment (- polarity)
	while (c < 16) {
		writeData(gmctrn1[c]);
		c++;
	}
}

void initLCD() {

	writeCommand(SWRESET);
	delay(20);
	writeCommand(SLEEPOUT);
	delay(20); // driver is doing self check, but seems to be working fine without the delay
	writeCommand(COLMOD);
	writeData(0x05);// 16-bit mode
	writeCommand(MADCTL);

	setOrientation(ORIENTATION);

	//writeCommand(SETCON);
	//writeData(0x3F);
	gammaAdjustmentST7735();

	writeCommand(DISPON);// why? DISPON should be set on power up, sleep out should be enough
}

void setOrientation(u_char orientation) {

	writeCommand(MADCTL);

	switch (orientation) {
	case 1:
		writeData(0x68);
		_orientation = 1;
		break;
	case 2:
		writeData(0x08);
		_orientation = 2;
		break;
	case 3:
		writeData(0xA8);
		_orientation = 3;
		break;
	default:
		writeData(0xC8);
		_orientation = 0;
	}
}

#endif



/////////////////////////////////////////////////
// ILI9225 based display
/////////////////////////////////////////////////
#ifdef ILI9225

void gammaAdjustmentILI9225() {
	const u_char dataMSB[] = {0x00, 0x09, 0x0d, 0x09, 0x04, 0x05, 0x00, 0x00,
		0x17, 0x00};
	const u_char dataLSB[] = {0x03, 0x00, 0x05, 0x00, 0x07, 0x02, 0x00, 0x05,
		0x00, 0x1F};
	const u_char cmd = 0x50;
	u_char c = 0;
	while (c < 10) {
		writeCommand(cmd + c);
		writeData(dataMSB[c]);
		writeData(dataLSB[c]);
		c++;
	}
}

void setGRAMILI9225() {
	const u_char dataLSB[] = {0x00, 0xDB, 0x00, 0x00, 0xDB, 0x00, 0xAF, 0x00,
		0xDB, 0x00};
	const u_char cmd = 0x30;
	u_char c = 0;
	while (c < 10) {
		writeCommand(cmd + c);
		writeData(0x00);
		writeData(dataLSB[c]);
		c++;
	}
}

void initLCD() {

	writeCommand(0x28);
	delay(20);

	setOrientation(ORIENTATION);

	writeCommand(0x08);
	writeData(0x08); // set BP and FP
	writeData(0x08);
	writeCommand(0x0C);
	writeData(0x00);// RGB interface setting 0x0110 for RGB 18Bit and 0111for RGB16Bit
	writeData(0x00);
	writeCommand(0x0F);
	writeData(0x0b);// Set frame rate//0b01
	writeData(0x01);
	delay(5);
	writeCommand(0x10);
	writeData(0x0a);// Set SAP,DSTB,STB//0800
	writeData(0x00);
	writeCommand(0x11);
	writeData(0x10);// Set APON,PON,AON,VCI1EN,VC
	writeData(0x38);
	delay(5);
	writeCommand(0x12);
	writeData(0x11);// Internal reference voltage= Vci;
	writeData(0x21);
	writeCommand(0x13);
	writeData(0x00);// Set GVDD
	writeData(0x63);
	writeCommand(0x14);
	writeData(0x4b);// Set VCOMH/VCOML voltage//3944
	writeData(0x44);
	writeCommand(ILIGRAMADDRX);
	writeData(0x00);
	writeData(0x00);
	writeCommand(ILIGRAMADDRY);
	writeData(0x00);
	writeData(0x00);
	setGRAMILI9225();
	gammaAdjustmentILI9225();

	delay(5);
	writeCommand(0x07);
	writeData(0x10);
	writeData(0x17);

}

void setOrientation(u_char orientation) {

	writeCommand(0x01);

	switch (orientation) {
	case 1:
		writeData(0x03);
		_orientation = 1;
		break;
	case 2:
		writeData(0x02);
		_orientation = 2;
		break;
	case 3:
		writeData(0x00);
		_orientation = 3;
		break;
	default:
		writeData(0x01);
		_orientation = 0;
	}

	writeData(0x1C);

	writeCommand(0x02);
	writeData(0x01); // set 1 line inversion
	writeData(0x00);

	writeCommand(0x03);
	writeData(0x10);// set GRAM write direction and BGR=1.//1030

	switch (orientation) {
	case 1:
		writeData(0x38);
		_orientation = 1;
		break;
	case 2:
		writeData(0x40);
		_orientation = 2;
		break;
	case 3:
		writeData(0x48);
		_orientation = 3;
		break;
	default:
		writeData(0x30);
		_orientation = 0;
	}

}

#endif

