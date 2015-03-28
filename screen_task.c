#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
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

#include "tiva.h"
#include "lcd.h"
#include "graphics.h"
#include "color.h"
#include "typedefs.h"
#include <math.h>
#include <string.h>
#include <stdio.h>


//*****************************************************************************
//
// The stack size for the display task.
//
//*****************************************************************************
#define SCREENTASKSTACKSIZE        128         // Stack size in words

//*****************************************************************************
//
// The item size and queue size for the LED message queue.
//
//*****************************************************************************
#define SCREEN_ITEM_SIZE           sizeof(uint8_t)
#define SCREEN_QUEUE_SIZE          5

#define SCREEN_TOGGLE_DELAY        250

//*****************************************************************************
//
// The queue that holds messages sent to the LED task.
//
//*****************************************************************************
xQueueHandle g_pScreenQueue;


#define MAIN_MENU 0
#define ALG_DGT 1
#define MEASUREMENTS 2
#define TIM_DAT 3
#define PARAMS 4
#define STOP 5
#define BREAK 6
#define PI 3.1416

extern xSemaphoreHandle g_pUARTSemaphore;

int speed = 10;
int dir = 0;

int loop = 0;
int state = 0; //which screen showing on LCD
int menuOption = 0; //which option is currently highlighted on the menu
int adOption = 0; //current option highlighted for analog/digital
int measureOption = 0; //current option highlighted for measured parameters
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
			drawString(0, 20, FONT_LG, "Please selct:");
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
			drawString(0, 20, FONT_LG, "Please selct:");
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
			drawString(0, 20, FONT_LG, "Please selct:");
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
			drawString(0, 90, FONT_MD, "Change Date/Time");

			menuOption = 2;
			break;

		case 2:
			setColor(COLOR_BLACK);
			fillRect(0,85,160,105);

			setColor(COLOR_RED);
			fillRect(0,105,160,125);

			setColor(COLOR_WHITE);
			drawString(0, 90, FONT_MD, "Change Date/Time");
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
			drawString(0, 90, FONT_MD, "Change Date/Time");
			drawString(0, 110, FONT_MD, "Enter Vehicle Params");
			break;

		default:
			clearScreen(1);
			break;

	}
}

void measuredParameters(void) {

	switch (measureOption) {

		case 0: //init
			clearScreen(1);
			measureOption = 1;

			setColor(COLOR_RED);
			fillRect(0,25,160,45);

			setColor(COLOR_WHITE);
			drawString(0, 30, FONT_MD, "Speed");
			drawString(0, 50, FONT_MD, "Accerlation");
			drawString(0, 70, FONT_MD, "G-Force");
			drawString(0, 90, FONT_MD, "Distance");
			drawString(0, 110, FONT_MD, "Back");

			break;

		case 1:
			measureOption = 2;

			setColor(COLOR_BLACK);
			fillRect(0,25,160,45);

			setColor(COLOR_RED);
			fillRect(0,45,160,65);

			setColor(COLOR_WHITE);
			drawString(0, 30, FONT_MD, "Speed");
			drawString(0, 50, FONT_MD, "Accerlation");
			break;

		case 2:
			measureOption = 3;

			setColor(COLOR_BLACK);
			fillRect(0,45,160,65);

			setColor(COLOR_RED);
			fillRect(0,65,160,85);

			setColor(COLOR_WHITE);
			drawString(0, 50, FONT_MD, "Accerlation");
			drawString(0, 70, FONT_MD, "G-Force");
			break;

		case 3:
			measureOption = 4;

			setColor(COLOR_BLACK);
			fillRect(0,65,160,85);

			setColor(COLOR_RED);
			fillRect(0,85,160,105);

			setColor(COLOR_WHITE);
			drawString(0, 70, FONT_MD, "G-Force");
			drawString(0, 90, FONT_MD, "Distance");
			break;

		case 4:
			measureOption = 5;

			setColor(COLOR_BLACK);
			fillRect(0,85,160,105);

			setColor(COLOR_RED);
			fillRect(0,105,160,125);

			setColor(COLOR_WHITE);
			drawString(0, 90, FONT_MD, "Distance");
			drawString(0, 110, FONT_MD, "Back");
			break;

		case 5:
			measureOption = 1;

			setColor(COLOR_BLACK);
			fillRect(0,105,160,125);

			setColor(COLOR_RED);
			fillRect(0,25,160,45);

			setColor(COLOR_WHITE);
			drawString(0, 30, FONT_MD, "Speed");
			drawString(0, 110, FONT_MD, "Back");
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

void speedAnalog(void) {

	double angle;
	int x,y;

	clearScreen(1);

	drawString(50, 15, FONT_LG, "Speed");
	drawCircle(80,125, 70);
	drawCircle(80,125, 2);

	drawString(63, 68, FONT_SM, "50 km/hr");
	drawString(20, 118, FONT_SM, "0 km/hr");
	drawString(90, 118, FONT_SM, "100 km/hr");

	drawString(79, 55, FONT_SM, "|");
	drawString(10, 124, FONT_SM, "--");
	drawString(140, 124, FONT_SM, "--");


	if (dir == 0) {
		if (speed == 90) {
			dir = 1;
		}
		speed += 10;
	} else {
		if (speed == 10) {
			dir = 0;
		}
		speed -= 10;
	}

	angle = 1.8*speed;

	x = 80 - 55*cos(angle*PI/180);
	y = 125 - 55*sin(angle*PI/180);

	drawLine(x,y,80,125);
	delay(100);
}

void speedDigital(void) {

	char buffer[] =  "50 km/hr";

	clearScreen(1);

	drawString(50, 30, FONT_LG, "Speed");

	if (dir == 0) {
		if (speed == 90) {
			dir = 1;
		}
		sprintf(buffer, "%d km/hr", speed);
		drawString(40, 80, FONT_LG, buffer);
		speed += 10;
	} else {
		if (speed == 10) {
			dir = 0;
		}
		sprintf(buffer, "%d km/hr", speed);
		drawString(40, 80, FONT_LG, buffer);
		speed -= 10;
	}

	delay(100);
}

void keyboard2dig(void) {


}

static void
ScreenTask(void *pvParameters)
{
	portTickType ui32WakeTime;
	uint32_t ui32ScreenToggleDelay;
	uint8_t  i8ButtonMessage;
	ui32WakeTime = xTaskGetTickCount();
	ui32ScreenToggleDelay = SCREEN_TOGGLE_DELAY;

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

            		case MEASUREMENTS:
            			measuredParameters();
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

            		default:
            			clearScreen(1);

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
            				state = ALG_DGT;
            				algDgtScreen();

            			} else if (menuOption == 1) {
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
            				measureOption = 0;
            				state = MEASUREMENTS;
            				measuredParameters();
            			}
            			break;

            		case MEASUREMENTS:
            			if (measureOption == 5) {
            				adOption = 0;
            				state = ALG_DGT;
            				algDgtScreen();

            			} else if (adOption == 1) {
            				if (measureOption == 1) {
            					loop = 11;
            					state = BREAK;
            				} else if (measureOption == 2) {
            					//acceleration
            				} else if (measureOption == 3) {
            					//g-force
            				} else if (measureOption == 4) {
            					//distance
            				}


            			} else if (adOption == 2) {
            				if (measureOption == 1) {
								loop = 21;
								state = BREAK;
							} else if (measureOption == 2) {

							} else if (measureOption == 3) {

							} else if (measureOption == 4) {

							}

            			}
            			break;

            		case BREAK:
            			loop = 0;
            			measureOption = 0;
            			state = MEASUREMENTS;
            			measuredParameters();
            			break;

            		case TIM_DAT:
            			if(tdOption == 3) {
							menuOption = 4;
							state = MAIN_MENU;
							menuScreen();
						} else {
							//keyboard
						}
						break;

            		case PARAMS:
						if(dmOption == 3) {
							menuOption = 4;
							state = MAIN_MENU;
							menuScreen();
						} else {
							//keyboard
						}
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

        if (loop == 11)
        	speedDigital();
        else if (loop == 21)
        	speedAnalog();


        vTaskDelayUntil(&ui32WakeTime, ui32ScreenToggleDelay / portTICK_RATE_MS);

    }
}

uint32_t ScreenTaskInit(void) {
	xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
	UARTprintf("About to menuTask init\n");
	xSemaphoreGive(g_pUARTSemaphore);

	initTiva();
	initLCD();

	setOrientation(3);
	clearScreen(1);

	setColor(COLOR_RED);
	fillRect(0,45,160,65);

	setColor(COLOR_WHITE);
	drawString(30, 20, FONT_LG, "Main Menu");
	drawString(0, 50, FONT_MD, "Start Journey");
	drawString(0, 70, FONT_MD, "Stop Journey");
	drawString(0, 90, FONT_MD, "Change Date/Time");
	drawString(0, 110, FONT_MD, "Enter Vehicle Params");

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
