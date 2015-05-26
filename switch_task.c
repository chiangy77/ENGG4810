//*****************************************************************************
//
// switch_task.c - A simple switch task to process the buttons.
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
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"
#include "switch_task.h"
#include "led_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//*****************************************************************************
//
// The stack size for the display task.
//
//*****************************************************************************
#define SWITCHTASKSTACKSIZE        64        // Stack size in words

extern xQueueHandle g_pLEDQueue;
extern xQueueHandle g_pScreenQueue;
extern xQueueHandle g_pRTCQueue;
extern xSemaphoreHandle g_pUARTSemaphore;

//*****************************************************************************
//
// This task reads the buttons' state and passes this information to LEDTask.
//
//*****************************************************************************
static void
SwitchTask(void* pvParameters)
{
	portTickType ui16LastTime;
	uint32_t ui32SwitchDelay = 25;
	uint8_t ui8CurButtonState, ui8PrevButtonState;
	uint8_t ui8Message;
	uint8_t ui8MessageToScreen;
	uint8_t ui8MessageToRTC;
	ui8CurButtonState = ui8PrevButtonState = 0;

	uint8_t flagTop, flagBottom, flagTopPrev, flagBottomPrev;
	//
	// Get the current tick count.
	//
	ui16LastTime = xTaskGetTickCount();

	//
	// Loop forever.
	//
	while(1)
		{
			//
			// Poll the debounced state of the buttons.
			//
			ui8CurButtonState = ButtonsPoll(0, 0);
			flagTop = GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_6);
			flagBottom = GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_7);

			//
			// Check if previous debounced state is equal to the current state.
			//
			if(ui8CurButtonState != ui8PrevButtonState || flagBottom != flagBottomPrev || flagTop != flagTopPrev)
				{
					ui8PrevButtonState = ui8CurButtonState;
					flagBottomPrev = flagBottom;
					flagTopPrev = flagTop;

					//
					// Check to make sure the change in state is due to button press
					// and not due to button release.
					//
					if((ui8CurButtonState & ALL_BUTTONS) != 0)
						{
							if((ui8CurButtonState & ALL_BUTTONS) == LEFT_BUTTON || flagBottom == 1)
								{
									ui8Message = LEFT_BUTTON;
									ui8MessageToScreen = LEFT_BUTTON;
									ui8MessageToRTC = LEFT_BUTTON;
									//
									// Guard UART from concurrent access.
									//
									xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
									UARTprintf("Left Button is pressed.\n");
									xSemaphoreGive(g_pUARTSemaphore);
								}
							else if((ui8CurButtonState & ALL_BUTTONS) == RIGHT_BUTTON || flagTop == 1)
								{
									ui8Message = RIGHT_BUTTON;
									ui8MessageToScreen = RIGHT_BUTTON;
									ui8MessageToRTC = RIGHT_BUTTON;
									//
									// Guard UART from concurrent access.
									//
									xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
									UARTprintf("Right Button is pressed.\n");
									xSemaphoreGive(g_pUARTSemaphore);
								}

							//
							// Pass the value of the button pressed to LEDTask.
							//
							if(xQueueSend(g_pRTCQueue, &ui8MessageToRTC, portMAX_DELAY) !=
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

							//
							// Pass the value of the button pressed to LEDTask.
							//
							if(xQueueSend(g_pScreenQueue, &ui8MessageToScreen, portMAX_DELAY) !=
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

							if(xQueueSendToFront(g_pLEDQueue, &ui8Message, portMAX_DELAY) !=
							        pdPASS)
								{
									//
									// Error. The queue should never be full. If so print the
									// error message on UART and wait for ever.
									//
									UARTprintf("LED Queue full. This should never happen.\n");

									while(1)
										{
										}
								}
						}
				}

			//
			// Wait for the required amount of time to check back.
			//
			vTaskDelayUntil(&ui16LastTime, ui32SwitchDelay / portTICK_RATE_MS);
		}
}

//*****************************************************************************
//
// Initializes the switch task.
//
//*****************************************************************************
uint32_t
SwitchTaskInit(void)
{
	//
	// Unlock the GPIO LOCK register for Right button to work.
	//
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0xFF;
	//
	// Initialize the buttons
	//
	ButtonsInit();

	ROM_SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_6);
	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_7);

	//
	// Create the switch task.
	//
	if(xTaskCreate(SwitchTask, (signed portCHAR*)"Switch",
	               SWITCHTASKSTACKSIZE, NULL, tskIDLE_PRIORITY +
	               PRIORITY_SWITCH_TASK, NULL) != pdTRUE)
		{
			return(1);
		}

	//
	// Success.
	//
	return(0);
}
