//*****************************************************************************
//
// led_task.c - A simple flashing LED task.
//
// Copyright (c) 2012-2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 1.1 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "drivers/rgb.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"
#include "RTC_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "RTC.h"
#include "screen_task.h"

//*****************************************************************************
//
// The stack size for the LED toggle task.
//
//*****************************************************************************
#define RTCTASKSTACKSIZE        128         // Stack size in words

//*****************************************************************************
//
// The item size and queue size for the LED message queue.
//
//*****************************************************************************
#define RTC_ITEM_SIZE           sizeof(uint8_t)
#define RTC_QUEUE_SIZE          5

//*****************************************************************************
//
// Default LED toggle delay value. LED toggling frequency is twice this number.
//
//*****************************************************************************
#define RTC_TOGGLE_DELAY        250

//*****************************************************************************
//
// The queue that holds messages sent to the LED task.
//
//*****************************************************************************
xQueueHandle g_pRTCQueue;


extern xSemaphoreHandle g_pUARTSemaphore;
//extern xSemaphoreHandle g_pSPISemaphore;

//*****************************************************************************
//
// This task toggles the user selected LED at a user selected frequency. User
// can make the selections by pressing the left and right buttons.
//
//*****************************************************************************
static void
RTCTask(void* pvParameters)
{
	portTickType ui32WakeTime;
	uint32_t ui32RTCToggleDelay;
	uint32_t ui32RcvDat, tYear=0;
	uint8_t i8Message, count, flag=0;
	int tHour=0, tMinute=0, tMonth=0, tSecond=0, tDay=0;

	// Get the current tick count.
	ui32WakeTime = xTaskGetTickCount();
	count = 0;

	// Loop forever.
	while(1) {
		//
		// Read the next message, if available on queue.
		//
		if(xQueueReceive(g_pRTCQueue, &i8Message, 20) == pdPASS)	{

			if(i8Message == LEFT_BUTTON) {

			}

			if(i8Message == RIGHT_BUTTON) {

			}

			//Reset count to 0
			count = 0;
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("Count reset to 0\n");
			xSemaphoreGive(g_pUARTSemaphore);


			if (flag) {
				flag = 0;
			}
		}

		if (state == 1 || state == 6)
			count = 0;

		if (count > 10) {

			if (!flag) {
				flag = 1;
				state = FROM_RTC;
			}

			getTime(&tHour, &tMinute, &tSecond);
			vTaskDelayUntil(&ui32WakeTime, 50);
			getDate(&tYear, &tMonth, &tDay);
			vTaskDelayUntil(&ui32WakeTime, 50);

			displayTimeDate(tYear, tMonth, tDay, tHour, tMinute, tSecond);
			while(SSIDataGetNonBlocking(SSI2_BASE, &ui32RcvDat)) {}
		}

		count++;

		vTaskDelayUntil(&ui32WakeTime, 1000 / portTICK_RATE_MS);
	}
}

//*****************************************************************************
//
// Initializes the LED task.
//
//*****************************************************************************
uint32_t
RTCTaskInit(void)
{

	// Create a queue for sending messages to the LED task.
	g_pRTCQueue = xQueueCreate(RTC_QUEUE_SIZE, RTC_ITEM_SIZE);

	//
	// Create the LED task.
	//
	if(xTaskCreate(RTCTask, (signed portCHAR*)"RTC", RTCTASKSTACKSIZE, NULL,
	               tskIDLE_PRIORITY + PRIORITY_RTC_TASK, NULL) != pdTRUE) {
			return(1);
	}

	ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / 1600);
	xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
	UARTprintf("RTCTask init return 0 clock speed: %d\n", SysCtlClockGet());
	xSemaphoreGive(g_pUARTSemaphore);

	//
	// Success.
	//
	return(0);
}
