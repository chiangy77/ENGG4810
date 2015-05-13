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

void initTiva() {

	// run @80MHz, use 16MHz xtal
	ROM_SysCtlClockSet (
			SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
					| SYSCTL_XTAL_16MHZ);

	ROM_SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB);
	ROM_SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOE);
	ROM_SysCtlPeripheralEnable (SYSCTL_PERIPH_SSI2);
	ROM_SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOA);

	ROM_GPIOPinTypeGPIOOutput (GPIO_PORTA_BASE, LCD_CS_PIN); //A4 = CS
	ROM_GPIOPinTypeGPIOOutput (GPIO_PORTE_BASE, LCD_DC_PIN); //E4 = Data
	ROM_GPIOPinConfigure (GPIO_PB4_SSI2CLK);				 //B4 = CLK
	ROM_GPIOPinTypeSSI (GPIO_PORTB_BASE, GPIO_PIN_4);
	ROM_GPIOPinConfigure (GPIO_PB7_SSI2TX);					 //B7 = MOSI
	ROM_GPIOPinTypeSSI (GPIO_PORTB_BASE, GPIO_PIN_7);

	ROM_SSIConfigSetExpClk (SSI2_BASE, ROM_SysCtlClockGet (),
			SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 16000000, 8);

	ROM_SSIEnable (SSI2_BASE);
}

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

void delay(u_char x10ms) {
	 ROM_SysCtlDelay( (ROM_SysCtlClockGet()/(3*100))*x10ms ) ;
}

