#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/fpu.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "drivers/rgb.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"
#include "screen_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "fatfs/src/ff.h"
#include "fatfs/src/diskio.h"

#include "tiva.h"
#include "lcd.h"
#include "graphics.h"
#include "color.h"
#include "typedefs.h"
#include <math.h>
#include "RTC.h"

#include "utils/uartstdio.h"
#include <stdio.h>
#include <string.h>

//*****************************************************************************
//
// The stack size for the display task.
//
//*****************************************************************************
#define SCREENTASKSTACKSIZE        1024         // Stack size in words

//*****************************************************************************
//
// The item size and queue size for the LED message queue.
//
//*****************************************************************************
#define SCREEN_ITEM_SIZE           sizeof(uint8_t)
#define SCREEN_QUEUE_SIZE          5

#define SCREEN_TOGGLE_DELAY        250

//*****************************************************************************
static FATFS g_sFatFs;

//*****************************************************************************
//
// A structure that holds a mapping between an FRESULT numerical code, and a
// string representation.  FRESULT codes are returned from the FatFs FAT file
// system driver.
//
//*****************************************************************************
typedef struct
{
	FRESULT iFResult;
	char *pcResultStr;
}
tFResultString;

//*****************************************************************************
//
// A macro to make it easy to add result codes to the table.
//
//*****************************************************************************
#define FRESULT_ENTRY(f)        { (f), (#f) }

//*****************************************************************************
//
// A table that holds a mapping between the numerical FRESULT code and it's
// name as a string.  This is used for looking up error codes for printing to
// the console.
//
//*****************************************************************************
tFResultString g_psFResultStrings[] =
{
		FRESULT_ENTRY(FR_OK),
		FRESULT_ENTRY(FR_DISK_ERR),
		FRESULT_ENTRY(FR_INT_ERR),
		FRESULT_ENTRY(FR_NOT_READY),
		FRESULT_ENTRY(FR_NO_FILE),
		FRESULT_ENTRY(FR_NO_PATH),
		FRESULT_ENTRY(FR_INVALID_NAME),
		FRESULT_ENTRY(FR_DENIED),
		FRESULT_ENTRY(FR_EXIST),
		FRESULT_ENTRY(FR_INVALID_OBJECT),
		FRESULT_ENTRY(FR_WRITE_PROTECTED),
		FRESULT_ENTRY(FR_INVALID_DRIVE),
		FRESULT_ENTRY(FR_NOT_ENABLED),
		FRESULT_ENTRY(FR_NO_FILESYSTEM),
		FRESULT_ENTRY(FR_MKFS_ABORTED),
		FRESULT_ENTRY(FR_TIMEOUT),
		FRESULT_ENTRY(FR_LOCKED),
		FRESULT_ENTRY(FR_NOT_ENOUGH_CORE),
		FRESULT_ENTRY(FR_TOO_MANY_OPEN_FILES),
		FRESULT_ENTRY(FR_INVALID_PARAMETER),
};

//*****************************************************************************
//
// A macro that holds the number of result codes.
//
//*****************************************************************************
#define NUM_FRESULT_CODES       (sizeof(g_psFResultStrings) /                 \
		sizeof(tFResultString))

//*****************************************************************************
//
// This function returns a string representation of an error code that was
// returned from a function call to FatFs.  It can be used for printing human
// readable error messages.
//
//*****************************************************************************
const char *
StringFromFResult(FRESULT iFResult)
{
	uint_fast8_t ui8Idx;

	//
	// Enter a loop to search the error code table for a matching error code.
	//
	for(ui8Idx = 0; ui8Idx < NUM_FRESULT_CODES; ui8Idx++)
	{
		//
		// If a match is found, then return the string name of the error code.
		//
		if(g_psFResultStrings[ui8Idx].iFResult == iFResult)
		{
			return(g_psFResultStrings[ui8Idx].pcResultStr);
		}
	}

	//
	// At this point no matching code was found, so return a string indicating
	// an unknown error.
	//
	return("UNKNOWN ERROR CODE");
}

//*****************************************************************************
//
// The queue that holds messages sent to the LED task.
//
//*****************************************************************************
xQueueHandle g_pScreenQueue;

extern xSemaphoreHandle g_pUARTSemaphore;
//extern xSemaphoreHandle g_pSPISemaphore;

int speed = 10;
int accel = 5;
int gForce = 1;
int odom = 0;
int dir = 0;

FIL fil;

int state = 0; //which screen showing on LCD
int menuOption = 0; //which option is currently highlighted on the menu
int adOption = 0; //current option highlighted for analog/digital
int tdOption = 0; //time and date
int dmOption = 0; //diameter and mass

void algDgtScreen(void) {

	switch (adOption) {

		case 0: //init
			clearScreen(1);
			adOption = 1;

			setColor(COLOR_RED);
			fillRect(0,45,160,65);

			setColor(COLOR_WHITE);
			drawString(0, 20, FONT_LG, "Please select");
			drawString(0, 50, FONT_MD, "Digital");
			drawString(0, 70, FONT_MD, "Analog");
			drawString(0, 90, FONT_MD, "Back");
			break;

		case 1:
			adOption = 2;

			setColor(COLOR_BLACK);
			fillRect(0,45,160,65);

			setColor(COLOR_RED);
			fillRect(0,65,160,85);

			setColor(COLOR_WHITE);
			drawString(0, 50, FONT_MD, "Digital");
			drawString(0, 70, FONT_MD, "Analog");
			break;
		case 2:
			adOption = 3;

			setColor(COLOR_BLACK);
			fillRect(0,65,160,85);

			setColor(COLOR_RED);
			fillRect(0,85,160,105);

			setColor(COLOR_WHITE);
			drawString(0, 70, FONT_MD, "Analog");
			drawString(0, 90, FONT_MD, "Back");
			break;

		case 3:
			adOption = 1;

			setColor(COLOR_BLACK);
			fillRect(0,85,160,105);

			setColor(COLOR_RED);
			fillRect(0,45,160,65);

			setColor(COLOR_WHITE);
			drawString(0, 50, FONT_MD, "Digital");
			drawString(0, 90, FONT_MD, "Back");
			break;

		default:
			clearScreen(1);
			break;
	}
}

void timDatScreen(void) {

	switch (tdOption) {

		case 0: //init
			clearScreen(1);
			tdOption = 1;

			setColor(COLOR_RED);
			fillRect(0,45,160,65);

			setColor(COLOR_WHITE);
			drawString(0, 20, FONT_LG, "Please select");
			drawString(0, 50, FONT_MD, "Time");
			drawString(0, 70, FONT_MD, "Date");
			drawString(0, 90, FONT_MD, "Back");
			break;

		case 1:
			tdOption = 2;

			setColor(COLOR_BLACK);
			fillRect(0,45,160,65);

			setColor(COLOR_RED);
			fillRect(0,65,160,85);

			setColor(COLOR_WHITE);
			drawString(0, 50, FONT_MD, "Time");
			drawString(0, 70, FONT_MD, "Date");
			break;
		case 2:
			tdOption = 3;

			setColor(COLOR_BLACK);
			fillRect(0,65,160,85);

			setColor(COLOR_RED);
			fillRect(0,85,160,105);

			setColor(COLOR_WHITE);
			drawString(0, 70, FONT_MD, "Date");
			drawString(0, 90, FONT_MD, "Back");
			break;

		case 3:
			tdOption = 1;

			setColor(COLOR_BLACK);
			fillRect(0,85,160,105);

			setColor(COLOR_RED);
			fillRect(0,45,160,65);

			setColor(COLOR_WHITE);
			drawString(0, 50, FONT_MD, "Time");
			drawString(0, 90, FONT_MD, "Back");
			break;

		default:
			clearScreen(1);
			break;
	}
}

void diaMassScreen(void) {

	switch (dmOption) {

		case 0: //init
			clearScreen(1);
			dmOption = 1;

			setColor(COLOR_RED);
			fillRect(0,45,160,65);

			setColor(COLOR_WHITE);
			drawString(0, 20, FONT_LG, "Please select");
			drawString(0, 50, FONT_MD, "Wheel Diameter");
			drawString(0, 70, FONT_MD, "Mass");
			drawString(0, 90, FONT_MD, "Back");
			break;

		case 1:
			dmOption = 2;

			setColor(COLOR_BLACK);
			fillRect(0,45,160,65);

			setColor(COLOR_RED);
			fillRect(0,65,160,85);

			setColor(COLOR_WHITE);
			drawString(0, 50, FONT_MD, "Wheel Diameter");
			drawString(0, 70, FONT_MD, "Mass");
			break;
		case 2:
			dmOption = 3;

			setColor(COLOR_BLACK);
			fillRect(0,65,160,85);

			setColor(COLOR_RED);
			fillRect(0,85,160,105);

			setColor(COLOR_WHITE);
			drawString(0, 70, FONT_MD, "Mass");
			drawString(0, 90, FONT_MD, "Back");
			break;

		case 3:
			dmOption = 1;

			setColor(COLOR_BLACK);
			fillRect(0,85,160,105);

			setColor(COLOR_RED);
			fillRect(0,45,160,65);

			setColor(COLOR_WHITE);
			drawString(0, 50, FONT_MD, "Wheel Diameter");
			drawString(0, 90, FONT_MD, "Back");
			break;

		default:
			clearScreen(1);
			break;
	}
}

void menuScreen(void) {

	switch (menuOption) {

		case 0:
			setColor(COLOR_BLACK);
			fillRect(0,45,160,65);

			setColor(COLOR_RED);
			fillRect(0,65,160,85);

			setColor(COLOR_WHITE);
			drawString(0, 50, FONT_MD, "Start Journey");
			drawString(0, 70, FONT_MD, "Stop Journey");

			menuOption = 1;
			break;

		case 1:
			setColor(COLOR_BLACK);
			fillRect(0,65,160,85);

			setColor(COLOR_RED);
			fillRect(0,85,160,105);

			setColor(COLOR_WHITE);
			drawString(0, 70, FONT_MD, "Stop Journey");
			drawString(0, 90, FONT_MD, "Change Time/Date");

			menuOption = 2;
			break;

		case 2:
			setColor(COLOR_BLACK);
			fillRect(0,85,160,105);

			setColor(COLOR_RED);
			fillRect(0,105,160,125);

			setColor(COLOR_WHITE);
			drawString(0, 90, FONT_MD, "Change Time/Date");
			drawString(0, 110, FONT_MD, "Enter Vehicle Params");

			menuOption = 3;
			break;

		case 3:
			setColor(COLOR_BLACK);
			fillRect(0,105,160,125);

			setColor(COLOR_RED);
			fillRect(0,45,160,65);

			setColor(COLOR_WHITE);
			drawString(0, 110, FONT_MD, "Enter Vehicle Params");
			drawString(0, 50, FONT_MD, "Start Journey");

			menuOption = 0;
			break;

		case 4:
			menuOption = 0;
			clearScreen(1);

			setColor(COLOR_RED);
			fillRect(0,45,160,65);

			setColor(COLOR_WHITE);
			drawString(30, 20, FONT_LG, "Main Menu");
			drawString(0, 50, FONT_MD, "Start Journey");
			drawString(0, 70, FONT_MD, "Stop Journey");
			drawString(0, 90, FONT_MD, "Change Time/Date");
			drawString(0, 110, FONT_MD, "Enter Vehicle Params");
			break;

		default:
			clearScreen(1);
			break;

	}
}

void stopScreen(void) {

	clearScreen(1);

	drawString(0, 50, FONT_MD, "Successfully written");
	drawString(0, 70, FONT_MD, "to micro SD card");
}

void showDigital(void) {
	char buffer[] =  "50 km/hr";

	clearScreen(1);

	drawString(7, 20, FONT_SM, "Speed");
	usnprintf (buffer, 20, "%d km/hr", speed);
	drawString(7, 40, FONT_SM, buffer);

	drawString(85, 20, FONT_SM, "Acceleration");
	usnprintf (buffer, 20, "%d m/s^2", accel);
	drawString(85, 40, FONT_SM, buffer);

	drawString(7, 80, FONT_SM, "G-Force");
	usnprintf (buffer, 20, "%d m/s^2", gForce);
	drawString(7, 100, FONT_SM, buffer);

	drawString(85, 80, FONT_SM, "Odometer");
	usnprintf (buffer, 20, "%d m", odom);
	drawString(85, 100, FONT_SM, buffer);

	delay(100);
}

void showAnalogSpeed(void) {
	double angle;
	int x,y;
	char buffer[] =  "50 km/hr";

	clearScreen(1);

	//show speed
	drawString(68, 5, FONT_SM, "Speed");
	drawHalfCircle(80,90, 70);

	drawString(63, 33, FONT_SM, "50 km/hr");
	drawString(15, 83, FONT_SM, "0 km/hr");
	drawString(95, 83, FONT_SM, "100 km/hr");

	drawString(79, 22, FONT_SM, "|");
	drawString(10, 89, FONT_SM, "--");
	drawString(140, 89, FONT_SM, "--");

	drawCircle(80,90, 2);

	drawHalfCircle(25,125,25); //acceleration
	drawHalfCircle(80,125,25); //g-force
	usnprintf (buffer, 20, "%d m", odom);
	drawString(118, 110, FONT_SM, buffer);


	//calculation for speed
	angle = 1.8*speed;

	x = 80 - 55*cos(angle*PI/180);
	y = 90 - 55*sin(angle*PI/180);

	drawLine(x,y,80,90);

	//calculation for acceleration
	angle = 9*accel;

	x = 25 - 20*cos(angle*PI/180);
	y = 125 - 20*sin(angle*PI/180);

	drawLine(x,y,25,125);

	//calculation for g-Force
	angle = 18*gForce;

	x = 80 - 20*cos(angle*PI/180);
	y = 125 - 20*sin(angle*PI/180);

	drawLine(x,y,80,125);
	delay(100);
}

void showAnalogAccel(void) {
	double angle;
	int x,y;
	char buffer[] =  "50 km/hr";

	clearScreen(1);

	//show speed
	drawString(45, 5, FONT_SM, "Acceleration");
	drawHalfCircle(80,90, 70);

	drawString(63, 33, FONT_SM, "10 m/s^2");
	drawString(15, 83, FONT_SM, "0 m/s^2");
	drawString(95, 83, FONT_SM, "20 m/s^2");

	drawString(79, 22, FONT_SM, "|");
	drawString(10, 89, FONT_SM, "--");
	drawString(140, 89, FONT_SM, "--");

	drawCircle(80,90, 2);

	drawHalfCircle(25,125,25); //acceleration
	drawHalfCircle(80,125,25); //g-force
	usnprintf (buffer, 20, "%d m", odom);
	drawString(118, 110, FONT_SM, buffer);


	//calculation for accel
	angle = 9*accel;

	x = 80 - 55*cos(angle*PI/180);
	y = 90 - 55*sin(angle*PI/180);

	drawLine(x,y,80,90);

	//calculation for g-Force
	angle = 18*gForce;

	x = 25 - 20*cos(angle*PI/180);
	y = 125 - 20*sin(angle*PI/180);

	drawLine(x,y,25,125);

	//calculation for speed
	angle = 1.8*speed;

	x = 80 - 20*cos(angle*PI/180);
	y = 125 - 20*sin(angle*PI/180);

	drawLine(x,y,80,125);

	delay(100);
}

void showAnalogGForce(void) {
	double angle;
	int x,y;
	char buffer[] =  "50 km/hr";

	clearScreen(1);

	//show speed
	drawString(60, 5, FONT_SM, "G-Force");
	drawHalfCircle(80,90, 70);

	drawString(63, 33, FONT_SM, "5 m/s^2");
	drawString(15, 83, FONT_SM, "0 m/s^2");
	drawString(95, 83, FONT_SM, "10 m/s^2");

	drawString(79, 22, FONT_SM, "|");
	drawString(10, 89, FONT_SM, "--");
	drawString(140, 89, FONT_SM, "--");

	drawCircle(80,90, 2);

	drawHalfCircle(25,125,25); //acceleration
	drawHalfCircle(80,125,25); //g-force
	usnprintf (buffer, 20, "%d m", odom);
	drawString(118, 110, FONT_SM, buffer);


	//calculation for g-force
	angle = 18*gForce;

	x = 80 - 55*cos(angle*PI/180);
	y = 90 - 55*sin(angle*PI/180);

	drawLine(x,y,80,90);


	//calculation for speed
	angle = 1.8*speed;

	x = 25 - 20*cos(angle*PI/180);
	y = 125 - 20*sin(angle*PI/180);

	drawLine(x,y,25,125);

	//calculation for acceleration
	angle = 9*accel;

	x = 80 - 20*cos(angle*PI/180);
	y = 125 - 20*sin(angle*PI/180);

	drawLine(x,y,80,125);

	delay(100);
}

void checkDate (int year, int *month, int *day) {

	switch (*month) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if (*day > 31) {
				*day = 1;
			}
			break;

		case 4:
		case 6:
		case 9:
		case 11:
			if (*day > 30) {
				*day = 1;
			}
			break;

		case 2:
			if (year%4 == 0) {
				if (*day > 29) {
					*day = 1;
				}
			} else {
				if (*day > 28) {
					*day = 1;
				}
			}
			break;

		case 13:
			*month = 1;
			break;

		default:
			*day = 1;
			*month = 1;
			break;
	}
}

void changeTime (int hour, int minute, int second, int cursor) {
	char buffer[] =  "00 : 00 : 00";

	clearScreen(1);
	usnprintf(buffer, 20, "%02d : %02d : %02d",hour, minute, second);
	drawString(10, 75, FONT_LG, buffer);

	if (cursor == 0)
		drawString(10, 80, FONT_LG, "__");
	else if (cursor == 1)
		drawString(70, 80, FONT_LG, "__");
	else if (cursor == 2)
		drawString(130, 80, FONT_LG, "__");
}

void changeDate (int year, int month, int day, int cursor) {
	char buffer[] =  "01/01/1994";

	clearScreen(1);
	usnprintf(buffer, 20, "%02d/%02d/%04d", day, month, year);
	drawString(10, 75, FONT_LG, buffer);

	if (cursor == 0)
		drawString(10, 80, FONT_LG, "__");
	else if (cursor == 1)
		drawString(45, 80, FONT_LG, "__");
	else if (cursor == 2)
		drawString(108, 80, FONT_LG, "_");
	else if (cursor == 3)
		drawString(120, 80, FONT_LG, "_");
}

void changeDiam (int diameter) {
	char buffer[] =  "600 mm";

	clearScreen(1);
	usnprintf(buffer, 20, "%d mm", diameter);
	drawString(10, 75, FONT_LG, buffer);
}

void displayTimeDate (int year, int month, int day, int hour, int minute, int second) {
	char buffer [] = "00 : 00 : 00";

	clearScreen(1);
	usnprintf(buffer, 20, "%02d : %02d : %02d",hour, minute, second);
	drawString(10, 25, FONT_LG, buffer);

	usnprintf(buffer, 20, "%02d/%02d/%04d", day, month, year);
	drawString(10, 95, FONT_LG, buffer);
}

void changeMass (int mass) {
	char buffer[] =  "1000 kg";

	clearScreen(1);
	usnprintf(buffer, 20, "%d kg", mass);
	drawString(10, 75, FONT_LG, buffer);
}

void hardcodeParams (void) {

	if (dir == 0) {
		if (speed == 90) {
			dir = 1;
		}
		speed += 10;
		accel += 1;
		gForce += 1;
		odom += 1;
	} else {
		if (speed == 10) {
			dir = 0;
		}
		speed -= 10;
		accel -= 1;
		gForce -= 1;
		odom += 2;
	}
}

void checkSDCard(void) {

	uint32_t ui32RcvDat;
	FRESULT iFResult;
	uint32_t count = 8*512;

	//taskENTER_CRITICAL();

	while(SSIDataGetNonBlocking(SSI2_BASE, &ui32RcvDat)) {}

	ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / 100);

	iFResult = f_mount(0, &g_sFatFs);
	if(iFResult != FR_OK)
	{       xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("f_mount error: %s\n", StringFromFResult(iFResult));
			xSemaphoreGive(g_pUARTSemaphore);
			//return(1);
	}
	else {
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("Mounted properly\n");
			xSemaphoreGive(g_pUARTSemaphore);
	}

	iFResult = f_open(&fil, "TestFile1.txt", FA_CREATE_ALWAYS|FA_WRITE);

	if(iFResult != FR_OK) {
		xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
		UARTprintf("fresult: %s\n", StringFromFResult(iFResult));
		xSemaphoreGive(g_pUARTSemaphore);

		drawString(0, 50, FONT_MD, "There is no SD");
		drawString(0, 70, FONT_MD, "card in the slot");
	}
	else{
		xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
		UARTprintf("Opened SD card\n");
		xSemaphoreGive(g_pUARTSemaphore);

		drawString(0, 50, FONT_MD, "There is an SD");
		drawString(0, 70, FONT_MD, "card in the slot");

		iFResult = f_close(&fil);
	}

	while(SSIDataGetNonBlocking(SSI2_BASE, &ui32RcvDat)) {}

	ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / 1600);

	//taskEXIT_CRITICAL();

}

void initWriteSDCard (int diameter, int mass, int hour, int minute, int second, char *textFileName) {

	uint32_t ui32RcvDat;
	char message[20];
	FRESULT iFResult;
	uint32_t count = 8*512;

	taskENTER_CRITICAL();

	while(SSIDataGetNonBlocking(SSI2_BASE, &ui32RcvDat)) {}

	ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / 100);

	memset(message, NULL, sizeof(message));

	iFResult = f_mount(0, &g_sFatFs);
	if(iFResult != FR_OK)
	{       xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("f_mount error: %s\n", StringFromFResult(iFResult));
			xSemaphoreGive(g_pUARTSemaphore);
			//return(1);
	}
	else {
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("Mounted properly\n");
			xSemaphoreGive(g_pUARTSemaphore);
	}

	iFResult = f_open(&fil, textFileName, FA_CREATE_ALWAYS|FA_WRITE);

	if(iFResult != FR_OK) {
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("fresult: %s\n", StringFromFResult(iFResult));
			xSemaphoreGive(g_pUARTSemaphore);
	}
	else{
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("Opened SD card\n");
			xSemaphoreGive(g_pUARTSemaphore);
	}

	usnprintf(message, 20, "%d %d %02d:%02d:%02d\r\n", diameter, mass, hour, minute, second);

	iFResult = f_write(&fil, message, strlen(message), &count);

	if(iFResult != FR_OK) {
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("Error writing to file: %s\n", StringFromFResult(iFResult));
			xSemaphoreGive(g_pUARTSemaphore);
	}
	else {
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("y\n");
			xSemaphoreGive(g_pUARTSemaphore);
	}

	iFResult = f_sync(&fil);

	while(SSIDataGetNonBlocking(SSI2_BASE, &ui32RcvDat)) {}

	ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / 1600);

	taskEXIT_CRITICAL();
}

void writeSDCard (int sp, int acceleration, int dist) {

	uint32_t ui32RcvDat;
	char message[20];
	FRESULT iFResult;
	uint32_t count = 8*512;

	taskENTER_CRITICAL();

	while(SSIDataGetNonBlocking(SSI2_BASE, &ui32RcvDat)) {}

	ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / 100);

	memset(message, NULL, sizeof(message));

	usnprintf(message, 20, "%d %d %d\r\n", sp, acceleration, dist);

	iFResult = f_write(&fil, message, strlen(message), &count);

	if(iFResult != FR_OK) {
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("Error writing to file: %s\n", StringFromFResult(iFResult));
			xSemaphoreGive(g_pUARTSemaphore);
	}
	else {
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("y\n");
			xSemaphoreGive(g_pUARTSemaphore);
	}

	iFResult = f_sync(&fil);

	while(SSIDataGetNonBlocking(SSI2_BASE, &ui32RcvDat)) {}

	ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / 1600);

	taskEXIT_CRITICAL();
}

void closeSDCard (int hour, int minute, int second, int year, int month, int day) {

	uint32_t ui32RcvDat;
	FRESULT iFResult;
	char message[50];
	uint32_t count = 8*512;

	taskENTER_CRITICAL();

	while(SSIDataGetNonBlocking(SSI2_BASE, &ui32RcvDat)) {}

	ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / 100);

	memset(message, NULL, sizeof(message));

	usnprintf(message, 50, "%02d/%02d/%02d %02d:%02d:%02d\r\n", day, month, year, hour, minute, second);

	iFResult = f_write(&fil, message, strlen(message), &count);

	if(iFResult != FR_OK) {
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("Error writing to file: %s\n", StringFromFResult(iFResult));
			xSemaphoreGive(g_pUARTSemaphore);
	}
	else {
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("Successfully written to SD card\n");
			xSemaphoreGive(g_pUARTSemaphore);
	}

	iFResult = f_close(&fil);

	if(iFResult != FR_OK) {
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("Error closing the file: %s\n", StringFromFResult(iFResult));
			xSemaphoreGive(g_pUARTSemaphore);
	}
	else {
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("Successfully closed the SD card\n");
			xSemaphoreGive(g_pUARTSemaphore);
	}

	while(SSIDataGetNonBlocking(SSI2_BASE, &ui32RcvDat)) {}

	ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / 1600);

	taskEXIT_CRITICAL();
}

uint8_t BCD(int input) {
	uint8_t upper, lower;

	upper = ((input%100)/10) << 4;
	lower = input%10;

	return (upper|lower);
}

uint8_t DCB(int input) {
	uint8_t upper, lower;

	upper = (input >> 4) * 10;
	lower = input & 0x0F;

	return (upper + lower);
}

static void
ScreenTask(void *pvParameters)
{
	portTickType ui32WakeTime;
	uint32_t ui32ScreenToggleDelay;
	uint8_t  i8ButtonMessage;
	ui32WakeTime = xTaskGetTickCount();
	ui32ScreenToggleDelay = SCREEN_TOGGLE_DELAY;

	int loop = 0;
	int analogTrack = 0, changeTimeTrack = 0, changeDateTrack = 0;
	int hour = 20, minute = 32, second = 55; //keep record of time
	int year = 2015, month = 4, day = 29;
	int mass = 800;
	int diameter = 600;
	int fileTrack = 1;

    //
    // Loop forever.
    //
    while(1)
    {
        //
        // Read the next message, if available on queue.
        //
        if(xQueueReceive(g_pScreenQueue, &i8ButtonMessage, 10) == pdPASS)
        {
        	//xSemaphoreTake(g_pSPISemaphore, portMAX_DELAY);
            //
            // If left button, update to next LED.
            //
            if(i8ButtonMessage == LEFT_BUTTON)
            {

            	switch(state) {
            		case MAIN_MENU:
            			menuScreen();
            			break;

            		case ALG_DGT:
            			algDgtScreen();
            			break;

            		case TIM_DAT:
            			timDatScreen();
            			break;

            		case PARAMS:
            			diaMassScreen();
            			break;

            		case STOP:
            			menuOption = 4;
            			menuScreen();
            			state = MAIN_MENU;
            			break;

            		case CHANGE_TIME:
            			if (changeTimeTrack == 0)
            				hour++;
							if (hour > 23)
								hour = 0;
            			else if (changeTimeTrack == 1)
            				minute++;
							if (minute > 59)
                				minute = 0;
            			else if (changeTimeTrack == 2)
            				second++;
                			if (second > 59)
                				second = 0;

            			changeTime(hour, minute, second, changeTimeTrack);
            			break;

            		case CHANGE_DATE:
            			if (changeDateTrack == 0) {
            				day++;
            				checkDate(year, &month, &day);
            			} else if (changeDateTrack == 1) {
            				month++;
            				checkDate(year, &month, &day);
            			} else if (changeDateTrack == 2) {
            				year += 10;
            			} else if (changeDateTrack == 3) {
            				year += 1;
            			}
            			if (year > 2100)
							year -= 100;

            			changeDate(year, month, day, changeDateTrack);
            			break;

            		case CHANGE_MASS:
            			mass += 100;
            			if (mass > 2000)
            				mass = 800;

            			changeMass(mass);
            			break;

            		case CHANGE_DIAM:
            			diameter += 20;
            			if (diameter > 800)
            				diameter = 600;

            			changeDiam(diameter);
            			break;

            		case BREAK:
            			if (loop == 21 && analogTrack == 0)
            				analogTrack = 1;
            			else if (loop == 21 && analogTrack == 1)
            				analogTrack = 2;
            			else if (loop == 21 && analogTrack == 2)
            				analogTrack = 0;
            			break;

            		case FROM_RTC:
						clearScreen(1);

						setColor(COLOR_RED);
						fillRect(0,45,160,65);

						setColor(COLOR_WHITE);
						drawString(30, 20, FONT_LG, "Main Menu");
						drawString(0, 50, FONT_MD, "Start Journey");
						drawString(0, 70, FONT_MD, "Stop Journey");
						drawString(0, 90, FONT_MD, "Change Time/Date");
						drawString(0, 110, FONT_MD, "Enter Vehicle Params");
						state = MAIN_MENU;
						menuOption = 0;
						break;

            		default:
            			break;

            	}

                //
                // Guard UART from concurrent access. Print the currently
                // blinking LED.
                //
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("State:%d\n", state);
                xSemaphoreGive(g_pUARTSemaphore);
            }

            //
            // If right button,
            if(i8ButtonMessage == RIGHT_BUTTON)
            {
                // Move onto next screen
            	switch(state) {

            		case MAIN_MENU:
            			if(menuOption == 0) {
            				adOption = 0;
            				getTime(&hour, &minute, &second);
            				if (fileTrack == 1)
            					initWriteSDCard(diameter, mass, hour, minute, second, "Journey15.txt");
            				else if (fileTrack == 2)
            					initWriteSDCard(diameter, mass, hour, minute, second, "Journey16.txt");
            				else if (fileTrack == 3)
								initWriteSDCard(diameter, mass, hour, minute, second, "Journey17.txt");
            				else if (fileTrack == 4)
            					initWriteSDCard(diameter, mass, hour, minute, second, "Journey18.txt");
            				fileTrack++;
            				if (fileTrack < 6) {
            					loop = 1;
								state = ALG_DGT;
								algDgtScreen();
            				}
							else {
								stopScreen();
								state = STOP;
							}

            			} else if (menuOption == 1) {
            				loop = 0;
            				getTime(&hour, &minute, &second);
            				getDate(&year, &month, &day);
                			closeSDCard(hour, minute, second, year, month, day);
            				state = STOP;
            				stopScreen();

            			} else if (menuOption == 2) {
            				state = TIM_DAT;
            				tdOption = 0;
            				timDatScreen();

            			} else if (menuOption == 3) {
            				state = PARAMS;
            				dmOption = 0;
            				diaMassScreen();

            			} else {
            				//throw error
            				clearScreen(1);
            			}
            			break;

            		case ALG_DGT:
            			if(adOption == 3) {
            				menuOption = 4;
            				state = MAIN_MENU;
            				menuScreen();
            			} else {

            				if (adOption == 1)
            					loop = 11; //digital
            				else
            					loop = 21; //analog

            				state = BREAK;
            			}
            			break;

            		case BREAK:
            			loop = 1;
            			adOption = 0;
						state = ALG_DGT;
						algDgtScreen();
            			break;

            		case TIM_DAT:
            			if(tdOption == 3) {
							menuOption = 4;
							state = MAIN_MENU;
							menuScreen();
						} else if (tdOption == 1) {
							getTime(&hour, &minute, &second);
							changeTimeTrack = 0;
							changeTime(hour, minute, second, changeTimeTrack);
							state = CHANGE_TIME;
						} else if (tdOption == 2) {
							getDate(&year, &month, &day);
							changeDateTrack = 0;
							changeDate(year, month, day, changeDateTrack);
							state = CHANGE_DATE;
						}
						break;

            		case CHANGE_TIME:
            			if (changeTimeTrack == 2) {
            				I2CSend(SLAVE_ADDR, 4, TIME_ADDR, BCD(second), BCD(minute), BCD(hour));

            				state = TIM_DAT;
							tdOption = 0;
							timDatScreen();
            			} else {
            				changeTimeTrack++;
            				changeTime(hour, minute, second, changeTimeTrack);
            			}

            			break;

            		case CHANGE_DATE:
            			if (changeDateTrack == 3) {
            				I2CSend(SLAVE_ADDR, 4, DATE_ADDR, BCD(day), BCD(month), BCD(year));

            				state = TIM_DAT;
							tdOption = 0;
							timDatScreen();
            			} else {
            				changeDateTrack++;
							changeDate(year, month, day, changeDateTrack);
            			}
            			break;

            		case PARAMS:
						if(dmOption == 3) {
							menuOption = 4;
							state = MAIN_MENU;
							menuScreen();
						} else if (dmOption == 1) {
							state = CHANGE_DIAM;
							changeDiam(diameter);
						} else if (dmOption == 2) {
							state = CHANGE_MASS;
							changeMass(mass);
						}
						break;

            		case CHANGE_MASS:
            			state = PARAMS;
						dmOption = 0;
						diaMassScreen();
            			break;

            		case CHANGE_DIAM:
            			state = PARAMS;
						dmOption = 0;
						diaMassScreen();
            			break;

            		case FROM_RTC:
            			clearScreen(1);

            			setColor(COLOR_RED);
            			fillRect(0,45,160,65);

            			setColor(COLOR_WHITE);
            			drawString(30, 20, FONT_LG, "Main Menu");
            			drawString(0, 50, FONT_MD, "Start Journey");
            			drawString(0, 70, FONT_MD, "Stop Journey");
            			drawString(0, 90, FONT_MD, "Change Time/Date");
            			drawString(0, 110, FONT_MD, "Enter Vehicle Params");
            			state = MAIN_MENU;
            			menuOption = 0;

            			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
						UARTprintf("In FROM_RTC\n");
						xSemaphoreGive(g_pUARTSemaphore);
            			break;

            		default:
						break;
            	}

                //
                // Guard UART from concurrent access. Print the currently
                // blinking frequency.
                //

				xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
				UARTprintf("State:%d \n", state);
				xSemaphoreGive(g_pUARTSemaphore);
            }
        }

        if (loop) {
        	hardcodeParams();
        	writeSDCard(speed, accel, odom);
        }

        if (loop == 11)
        	showDigital();
        else if (loop == 21 && analogTrack == 0)
        	showAnalogSpeed();
        else if (loop == 21 && analogTrack == 1)
			showAnalogAccel();
        else if (loop == 21 && analogTrack == 2)
			showAnalogGForce();

        //xSemaphoreGive(g_pSPISemaphore);

        vTaskDelayUntil(&ui32WakeTime, ui32ScreenToggleDelay / portTICK_RATE_MS);

    }
}

uint32_t ScreenTaskInit(void) {
	int tHour=0, tMinute=0, tMonth=0, tSecond=0, tDay=0, tYear=0;

	xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
	UARTprintf("About to menuTask init\n");
	xSemaphoreGive(g_pUARTSemaphore);

	initTiva();
	initLCD();
	InitI2C0();

	setOrientation(3);
	clearScreen(1);
	setColor(COLOR_WHITE);

	getTime(&tHour, &tMinute, &tSecond);
	getDate(&tYear, &tMonth, &tDay);

	displayTimeDate(tYear, tMonth, tDay, tHour, tMinute, tSecond);

	checkSDCard();

	state = FROM_RTC;

	g_pScreenQueue = xQueueCreate(SCREEN_QUEUE_SIZE, SCREEN_ITEM_SIZE);

	if(xTaskCreate(ScreenTask, (signed portCHAR*)"Screen", SCREENTASKSTACKSIZE,
	               NULL,
	               tskIDLE_PRIORITY + PRIORITY_SCREEN_TASK, NULL) != pdTRUE)
		{
		xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("menuTask init return 1\n");
			xSemaphoreGive(g_pUARTSemaphore);
			return(1);
	    }
	xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
		UARTprintf("menuTask init return 0\n");
		xSemaphoreGive(g_pUARTSemaphore);

	    //
	    // Success.
	    //
	    return(0);
}
