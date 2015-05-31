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

#include "utils/uartstdio.h"
#include <stdio.h>

#include "radio.h"
#include "nRF24L01.h"

#include "FreeRTOS.h"
#include "semphr.h"

extern xSemaphoreHandle g_pUARTSemaphore;
extern xSemaphoreHandle g_pSPISemaphore;

/*
 * radio.c
 *
 *  Created on: 21 Apr 2015
 *      Author: sharpestu
 */


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

void setCE(unsigned char val)
{
  if (val>0)
  GPIOPinWrite(GPIO_PORTE_BASE, NRF_CE_PIN, NRF_CE_PIN); //CE HIGH
  else
  GPIOPinWrite(GPIO_PORTE_BASE, NRF_CE_PIN ,0); //CE LOW
}

void setCSN(unsigned char val)
{
  if (val>0)
  GPIOPinWrite(GPIO_PORTE_BASE, NRF_CSN_PIN, NRF_CSN_PIN); //CSN HIGH
 else
  GPIOPinWrite(GPIO_PORTE_BASE, NRF_CSN_PIN, 0); //CSN LOW
}


uint8_t sendChar(unsigned char c)
{
  uint32_t temp = 0;
  uint8_t result = 0;

  SSIDataPut(SSI1_BASE, c);
  while(SSIBusy(SSI1_BASE)) {}

  SSIDataGet(SSI1_BASE, &temp);

  result = temp &= 0x00FF;

  return result;
}


void writeReg(unsigned char addr, unsigned char val)
{
  setCSN(0);
  sendChar(W_REGISTER | addr);
  sendChar(val);
  setCSN(1);
}

unsigned long readReg(unsigned char addr)
{
  uint8_t result = 0xff;

  setCSN(0);
  sendChar(R_REGISTER | addr);

  //while(SSIDataGetNonBlocking(SSI1_BASE, &result)){}

  result = sendChar(0xf0); //whatever value, used 0xf0 because it is easily visible with the oscilloscope

  //SSIDataGet(SSI1_BASE, &result);
  setCSN(1);

  //xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
  //UARTprintf("read Reg %x\n", result);
  //xSemaphoreGive(g_pUARTSemaphore);

  return result;

}

uint8_t SendRecv_Byte(uint8_t byte) {

	uint8_t rxbyte;

	uint32_t pui32DataTx[1];
	uint32_t pui32DataRx[1];

	pui32DataTx[0] = byte;

	SSIDataPut(SSI1_BASE, pui32DataTx[0]);

	while(SSIBusy(SSI1_BASE))
	    {
	    }

	SSIDataGet(SSI1_BASE, &pui32DataRx[0]);

	rxbyte = pui32DataRx[0] &= 0x00FF;

	return rxbyte;
}


void writeBuffer(uint8_t reg_addr, uint8_t *buffer, int buffer_len) {

	int i;

	setCSN(0); // CE Low

	SysCtlDelay(SysCtlClockGet() / 100 / 3);

	sendChar(W_REGISTER + reg_addr);

	SysCtlDelay(SysCtlClockGet() / 100 / 3);

	for (i = 0; i < buffer_len; ++i) {
		/* Return the Byte read from the SPI bus */
		sendChar(buffer[i]);
		SysCtlDelay(SysCtlClockGet() / 100 / 3);
	}

	setCSN(1); // CE High
	SysCtlDelay(SysCtlClockGet() / 100 / 3);
}

void readBuffer(uint8_t reg_addr, uint8_t *buffer, int buffer_len) {
	int i;

	setCSN(0);
	sendChar(reg_addr);

	for (i = 0; i < buffer_len; i++) {

		buffer[i] = sendChar(0xFF);
	}

	setCSN(1);
}


int recieve_packet(uint8_t *buffer)
{
	// Put into standby mode
	writeReg(CONFIG, 0x73);	//0x0f     	// Set PWR_UP bit, enable CRC(2 unsigned chars) & Prim:RX.
	setCE(1);

	int rec = 0;
	unsigned char status = readReg(STATUS);                  // read register STATUS's value

	if(status & 0x40) {

	    readBuffer(R_RX_PAYLOAD, buffer, 32);  // read playload to rx_buf
	    writeReg(FLUSH_RX, 0);                             // clear RX_FIFO
	    rec = 1;

	    setCE(0);

		writeReg(STATUS, status);                  // clear RX_DR or TX_DS or MAX_RT interrupt flag
	}


	//tastEXIT_CRITICAL();
	return rec;
}

void parse_packet(uint8_t *input, int16_t *output) {
	//X
	output[0] = (input[1]<<8)|input[0];
	/* The Y value is stored in values[2] and values[3] */
	output[1] = (input[3]<<8)|input[2];
	/* The Z value is stored in values[4] and values[5] */
	output[2] = (input[5]<<8)|input[4];
}

void nrfInit()
{
  uint32_t dummy = 0;
  //writeReg(CONFIG, 0x00);  // Deep power-down, everything disabled
  writeReg(EN_AA, 0x00);
  writeReg(EN_RXADDR, 0x01);
  writeReg(RF_SETUP, 0x00);
  writeReg(RX_PW_P0, 32);
  writeReg(RF_CH, 50);
  writeReg(RF_SETUP, 0x06);

  uint8_t tx_addr[] = {0x42, 0x65, 0x15, 0x41, 0x00};
  uint8_t rx_addr[] = {0x42, 0x64, 0x55, 0x68, 0x00};

  writeBuffer(RX_ADDR_P0, rx_addr, 5);
  writeBuffer(TX_ADDR, tx_addr, 5);

  writeReg(CONFIG, 0x02);

  //writeReg(STATUS, NRF24_IRQ_MASK);  // Clear all IRQs
  //writeReg(DYNPD, 0x03);
  //writeReg(FEATURE, EN_DPL);  // Dynamic payloads enabled by default

  SysCtlDelay(SysCtlClockGet() / 100 / 3); // grace time, never hurts
  while(SSIDataGetNonBlocking(SSI1_BASE, &dummy)) {}
}


int isAlive() {
	unsigned long aw;
	aw = readReg(SETUP_AW);

	return ((aw & 0xFC) == 0x00 && (aw & 0x03) != 0x00);
}


