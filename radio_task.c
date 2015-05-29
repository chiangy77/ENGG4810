/*
 * radio.c
 *
 *  Created on: 20 Apr 2015
 *      Author: sharpestu
 */


#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/ssi.h"
#include <driverlib/sysctl.h>
#include "drivers/rgb.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"
#include "radio_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "tiva.h"
#include "typedefs.h"
#include <math.h>

#include "utils/uartstdio.h"
#include <stdio.h>

#include "radio.h"
#include "nRF24L01.h"
#include "filtering.h"

//*****************************************************************************
//
// The stack size for the radio task.
//
//*****************************************************************************
#define RADIOTASKSTACKSIZE        512         // Stack size in words

extern xQueueHandle g_pScreenQueue;
extern xQueueHandle g_pScreenRadioQueue;

//*****************************************************************************
//
// The item size and queue size for the LED message queue.
//
//*****************************************************************************
//#define RADIO_ITEM_SIZE           sizeof(uint8_t)
//#define RADIO_QUEUE_SIZE          5

#define RADIO_TOGGLE_DELAY        10


extern xSemaphoreHandle g_pUARTSemaphore;
//extern xSemaphoreHandle g_pSPISemaphore;

//*****************************************************************************
//
// The queue that holds messages sent to the LED task.
//
//*****************************************************************************
xQueueHandle g_pRadioQueue;

static void RadioTask(void *pvParameters) {
	portTickType ui32WakeTime;
	uint32_t ui32RadioToggleDelay;
	//uint8_t  i8ButtonMessage;
	ui32WakeTime = xTaskGetTickCount();
	// Maybe change this??
	ui32RadioToggleDelay = RADIO_TOGGLE_DELAY;

	int counter=0;
    uint8_t packet[32], i;
    int16_t parsed[3], medianYAxis[3], averageYAxis[5], ui16MessageToScreen;
    int16_t medianYAxisLarge[5];
    //float movingAverageYAxis[5];
	//
	// Loop forever.
	//
	while(1)
	{


		for (i = 0; i < 32; ++i) {
			packet[i] = 0;
		}

		if (recieve_packet(packet)) {
			parse_packet(packet, parsed);

			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("%d, %d, %d,\n", parsed[0], parsed[1], parsed[2]);
			xSemaphoreGive(g_pUARTSemaphore);

			medianYAxis[counter%3] = parsed[1];
			//medianYAxisLarge[counter%5] = parsed[1];
			counter++;
			//ui16MessageToScreen = median(medianYAxis, counter);
			//ui16MessageToScreen = medianOf5Values(medianYAxisLarge, counter);
			averageYAxis[counter%5] = median(medianYAxis, counter);
			ui16MessageToScreen = convolution(averageYAxis, counter);

			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("%d, %d, %d, %d, %d, %d\n", averageYAxis[0], averageYAxis[1], averageYAxis[2], averageYAxis[3], averageYAxis[4], ui16MessageToScreen);
			xSemaphoreGive(g_pUARTSemaphore);
			//
			// Pass the value of the button pressed to ScrenTask.
			//
			if(xQueueSend(g_pScreenRadioQueue, &ui16MessageToScreen, portMAX_DELAY) !=
					pdPASS)
				{
					//
					// Error. The queue should never be full. If so print the
					// error message on UART and wait for ever.
					//
					UARTprintf("\nScreen Queue full. This should never happen.\n");

					while(1)
						{
						}
				}
		}

		//xSemaphoreGive(g_pSPISemaphore);



		//}
		vTaskDelayUntil(&ui32WakeTime, ui32RadioToggleDelay / portTICK_RATE_MS);

	}
}

uint32_t RadioTaskInit(void) {
	xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
	UARTprintf("About to radioTask init\n");
	xSemaphoreGive(g_pUARTSemaphore);

	//g_pRadioQueue = xQueueCreate(RADIO_QUEUE_SIZE, RADIO_ITEM_SIZE);

	initRadio();
	setCE(0);
	setCSN(1);
	nrfInit();


	if(xTaskCreate(RadioTask, (signed portCHAR*)"Radio", RADIOTASKSTACKSIZE,
	               NULL,
	               tskIDLE_PRIORITY + PRIORITY_RADIO_TASK, NULL) != pdTRUE)
		{
		xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("radioTask init return 1\n");
			xSemaphoreGive(g_pUARTSemaphore);
			return(1);
	    }
	xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
		UARTprintf("radioTask init return 0\n");
		xSemaphoreGive(g_pUARTSemaphore);

	    //
	    // Success.
	    //
	    return(0);
}



