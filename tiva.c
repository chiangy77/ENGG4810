#include "inc/tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "tiva.h"
#include "lcd.h"
#include "config.h"

////////////////////////////////////
// init
////////////////////////////////////

void initTiva() {

	// run @80MHz, use 16MHz xtal
	MAP_SysCtlClockSet (
			SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
					| SYSCTL_XTAL_16MHZ);

	MAP_SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB);
	MAP_SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOE);
	MAP_SysCtlPeripheralEnable (SYSCTL_PERIPH_SSI2);
	MAP_SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOA);

	MAP_GPIOPinTypeGPIOOutput (GPIO_PORTA_BASE, LCD_CS_PIN);
	MAP_GPIOPinTypeGPIOOutput (GPIO_PORTE_BASE, LCD_DC_PIN);
	MAP_GPIOPinConfigure (GPIO_PB4_SSI2CLK);
	MAP_GPIOPinTypeSSI (GPIO_PORTB_BASE, GPIO_PIN_4);
	MAP_GPIOPinConfigure (GPIO_PB7_SSI2TX);
	MAP_GPIOPinTypeSSI (GPIO_PORTB_BASE, GPIO_PIN_7);

	// ILI9340's max SPI clk is 15MHz, ILI9341's 10MHz
	// however, 320x240 LCD (ILI9341) works good @16M or higher
	MAP_SSIConfigSetExpClk (SSI2_BASE, MAP_SysCtlClockGet (),
			SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 16000000, 8);

	MAP_SSIEnable (SSI2_BASE);
}

////////////////////////////////////
// write & delay
////////////////////////////////////

// code duplication, but we are saving clock cycles by not passing dataCommand
void writeData(u_char data) {
	LCD_SELECT;
	LCD_DC_DATA;

	SSI2_DR_R = data;
	while (SSI2_SR_R & SSI_SR_BSY) {
	};

	LCD_DESELECT;
}

// code duplication, but we are saving clock cycles by not passing dataCommand
void writeCommand(u_char command) {
	LCD_SELECT;
	LCD_DC_CMD;

	SSI2_DR_R = command;
	while (SSI2_SR_R & SSI_SR_BSY) {
	};

	LCD_DESELECT;
}

/*
 * long delay
 */
void delay(u_char x10ms) {
	 MAP_SysCtlDelay( (MAP_SysCtlClockGet()/(3*100))*x10ms ) ;
}

