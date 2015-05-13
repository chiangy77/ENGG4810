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
#include "driverlib/pin_map.h"
#include "radio_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//#include "radio.h"
//#include "nRF24L01.h"

//*****************************************************************************
//
// The stack size for the display task.
//
//*****************************************************************************
#define RADIOTASKSTACKSIZE        64        // Stack size in words


extern xSemaphoreHandle g_pUARTSemaphore;

void initRadio() {

	// Init nRF

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); // SPI
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // CS, CE, IRQ

	ROM_GPIOPinConfigure(GPIO_PD0_SSI1CLK);
	ROM_GPIOPinConfigure(GPIO_PD2_SSI1RX);
	ROM_GPIOPinConfigure(GPIO_PD3_SSI1TX);
	ROM_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0);

	ROM_SSIConfigSetExpClk(SSI1_BASE, ROM_SysCtlClockGet(),
			 SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1000000, 8);

	ROM_SSIEnable(SSI1_BASE);

	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE,  NRF_CE_PIN);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, NRF_CSN_PIN);
	ROM_GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, NRF_IRQ_PIN);

	uint32_t clean = 0;
	while(SSIDataGetNonBlocking(SSI1_BASE, &clean)) {}

}

static void
RadioTask(void* pvParameters)
{
	portTickType ui16LastTime;
	uint32_t ui32SwitchDelay = 25;
//	uint8_t ui8CurButtonState, ui8PrevButtonState;
//	uint8_t ui8Message;
//	uint8_t ui8MessageToScreen;
//	ui8CurButtonState = ui8PrevButtonState = 0;
//	//
//	// Get the current tick count.
//	//
//	ui16LastTime = xTaskGetTickCount();

	//
	// Loop forever.
	//
	while(1)
		{
//			//
//			// Poll the debounced state of the buttons.
//			//
//			ui8CurButtonState = ButtonsPoll(0, 0);
//
//			//
//			// Check if previous debounced state is equal to the current state.
//			//
//			if(ui8CurButtonState != ui8PrevButtonState)
//				{
//					ui8PrevButtonState = ui8CurButtonState;
//
//					//
//					// Check to make sure the change in state is due to button press
//					// and not due to button release.
//					//
//					if((ui8CurButtonState & ALL_BUTTONS) != 0)
//						{
//							if((ui8CurButtonState & ALL_BUTTONS) == LEFT_BUTTON)
//								{
//									ui8Message = LEFT_BUTTON;
//									ui8MessageToScreen = LEFT_BUTTON;
//									//
//									// Guard UART from concurrent access.
//									//
//									xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
//									UARTprintf("Left Button is pressed.\n");
//									xSemaphoreGive(g_pUARTSemaphore);
//								}
//							else if((ui8CurButtonState & ALL_BUTTONS) == RIGHT_BUTTON)
//								{
//									ui8Message = RIGHT_BUTTON;
//									ui8MessageToScreen = RIGHT_BUTTON;
//									//
//									// Guard UART from concurrent access.
//									//
//									xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
//									UARTprintf("Right Button is pressed.\n");
//									xSemaphoreGive(g_pUARTSemaphore);
//								}
//
//							//
//							// Pass the value of the button pressed to LEDTask.
//							//
//							if(xQueueSend(g_pScreenQueue, &ui8MessageToScreen, portMAX_DELAY) !=
//							        pdPASS)
//								{
//									//
//									// Error. The queue should never be full. If so print the
//									// error message on UART and wait for ever.
//									//
//									UARTprintf("\nScreen Queue full. This should never happen.\n");
//
//									while(1)
//										{
//										}
//								}
//
//							if(xQueueSendToFront(g_pLEDQueue, &ui8Message, portMAX_DELAY) !=
//							        pdPASS)
//								{
//									//
//									// Error. The queue should never be full. If so print the
//									// error message on UART and wait for ever.
//									//
//									UARTprintf("LED Queue full. This should never happen.\n");
//
//									while(1)
//										{
//										}
//								}
//						}
//				}
//
//			//
//			// Wait for the required amount of time to check back.
//			//
			vTaskDelayUntil(&ui16LastTime, ui32SwitchDelay / portTICK_RATE_MS);
		}
}

//*****************************************************************************
//
// Initializes the switch task.
//
//*****************************************************************************
uint32_t
RadioTaskInit(void)
{
//	//
//	// Unlock the GPIO LOCK register for Right button to work.
//	//
//	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
//	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0xFF;
//	//
//	// Initialize the buttons
//	//
//	ButtonsInit();

	initRadio();

	//
	// Create the radio task.
	//
	if(xTaskCreate(RadioTask, (signed portCHAR*)"Radio",
	               RADIOTASKSTACKSIZE, NULL, tskIDLE_PRIORITY +
	               PRIORITY_RADIO_TASK, NULL) != pdTRUE)
		{
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("radioTask init return 1\n");
			xSemaphoreGive(g_pUARTSemaphore);
			return(1);
		} else {
			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
			UARTprintf("radioTask init return 0\n");
			xSemaphoreGive(g_pUARTSemaphore);
		}

	//
	// Success.
	//
	return(0);
}
