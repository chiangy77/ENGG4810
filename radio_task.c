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

#include "screen_task.h"
#include "radio.h"
#include "nRF24L01.h"
#include "filtering.h"

//*****************************************************************************
//
// The stack size for the radio task.
//
//*****************************************************************************
#define RADIOTASKSTACKSIZE        640        // Stack size in words

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
    uint8_t packet[32], i, freqFlag, freqFlagPrev;
    int16_t parsed[3], medianYAxis[3], averageYAxis[5];
    int16_t average;
//    uint32_t ui16MessageToScreen;

    float speed = 0, speedPrev=0;
    float accel = 0;
    uint8_t gForce = 0;
    float odom = 0;
    portTickType ui32BetweenTransmitTime, ui32BetweenTransmitTimePrev;
	uint32_t timeDif=0,time=0;

	struct packet sendPacket;

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
			time = xTaskGetTickCount()*portTICK_RATE_MS;
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("%d, %d, %d, time: %d\n", parsed[0], parsed[1], parsed[2], time);
			xSemaphoreGive(g_pUARTSemaphore);

			medianYAxis[counter%3] = parsed[1];

			counter++;

			average = median(medianYAxis, counter);

			//averageYAxis[counter%5] = median(medianYAxis, counter);
			//average = convolution(averageYAxis, counter);

			if (average > 0) {
				freqFlag = 1;
			} else {
				freqFlag = 0;
			}

			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("average: %d freqFlag: %d freqFlagPrev: %d\n", average, freqFlag, freqFlagPrev);
			xSemaphoreGive(g_pUARTSemaphore);


			if (freqFlag != freqFlagPrev) {

				freqFlagPrev = freqFlag;
				ui32BetweenTransmitTime = xTaskGetTickCount();
				timeDif = (ui32BetweenTransmitTime - ui32BetweenTransmitTimePrev)*portTICK_RATE_MS;

				speed = PI*600*3.6/timeDif/1.4;
				accel = (speed - speedPrev)/3.6*1000/timeDif;
				odom += sqrt((speed/3.6*timeDif/1000)*(speed/3.6*timeDif/1000));

				xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
				UARTprintf("timeDif: %d, speed: %d, speedPrev: %d, accel:%d, odom: %d\n", timeDif, (int8_t) speed, (int8_t) speedPrev, (int8_t) accel, (int16_t) odom);
				xSemaphoreGive(g_pUARTSemaphore);

				speedPrev = speed;
				ui32BetweenTransmitTimePrev = ui32BetweenTransmitTime;

				//ui16MessageToScreen = (uint8_t) speed << 24 | (int8_t) accel << 16 | (uint16_t) odom;
				sendPacket.speed = (uint8_t) speed;
				sendPacket.accel = (int8_t) accel;
				sendPacket.odom = (uint16_t) odom;

				xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
				UARTprintf("speed: %d accel: %d odom: %d\n", sendPacket.speed, sendPacket.accel, sendPacket.odom);
				xSemaphoreGive(g_pUARTSemaphore);
			}


			//
			// Pass the value of the button pressed to ScrenTask.
			//
			if(xQueueSend(g_pScreenRadioQueue, &sendPacket, portMAX_DELAY) !=
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
		UARTprintf("radioTask init return 0 clock speed: %d\n", SysCtlClockGet());
		xSemaphoreGive(g_pUARTSemaphore);

//		ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
//			                   SYSCTL_OSC_MAIN);
		ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / 1600);

	    //
	    // Success.
	    //
	    return(0);
}



