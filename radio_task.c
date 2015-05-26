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

//*****************************************************************************
//
// The stack size for the radio task.
//
//*****************************************************************************
#define RADIOTASKSTACKSIZE        128 * 4         // Stack size in words

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

	int i;
    uint8_t packet[32];
    int16_t parsed[3];

    //SysCtlClockSet(  SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL |
    //		        SYSCTL_XTAL_16MHZ  | SYSCTL_OSC_MAIN);


	//
	// Loop forever.
	//
	while(1)
	{
	//
	// Read the next message, if available on queue.
	//
		//if(xQueueReceive(g_pRadioQueue, &i8ButtonMessage, 10) == pdPASS)
		//{

		//xSemaphoreTake(g_pSPISemaphore, portMAX_DELAY);

		//uint8_t byte;
		//byte = SendRecv_Byte(0x41);
		//unsigned long aw;
		//aw = readReg(SETUP_AW);

		//if (isAlive()!=0) {
	//		xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
	//		//UARTprintf("Radio Recieved (%c), Reg %x\n", byte, aw);
			//UARTprintf("Radio Alive\n");
		//	xSemaphoreGive(g_pUARTSemaphore);
	//	}


		//UARTprintf("start 456\n");
		//xSemaphoreGive(g_pUARTSemaphore);

		//xSemaphoreTake(g_pSPISemaphore, portMAX_DELAY);

		for (i = 0; i < 32; ++i) {
			packet[i] = 0;
		}

		if (recieve_packet(packet)) {
			parse_packet(packet, parsed);

			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			//UARTprintf("Received Raw: ");
			//for (i = 0; i < 32; i++ ) {
			//	UARTprintf("%x ", packet[i]);
			//}
			UARTprintf("%d, %d, %d,\r", parsed[0], parsed[1], parsed[2]);
			//UARTprintf("\n\rRecieved: ");
			//for (i = 0; i < 32; i++ ) {
			//	UARTprintf("%c", packet[i]);
			//}
			//UARTprintf("\n\r");
			xSemaphoreGive(g_pUARTSemaphore);
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



