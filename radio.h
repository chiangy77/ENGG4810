/*
 * radio.h
 *
 *  Created on: 21 Apr 2015
 *      Author: sharpestu
 */

#ifndef RADIO_H_
#define RADIO_H_

void initRadio();
void setCE(unsigned char val);
void setCSN(unsigned char val);
uint8_t sendChar(unsigned char c);
void writeReg(unsigned char addr, unsigned char val);
unsigned long readReg(unsigned char addr);
void writeBuffer(uint8_t reg_addr, uint8_t *buffer, int buffer_len);
void readBuffer(uint8_t reg_addr, uint8_t *buffer, int buffer_len);
int isAlive();
int recieve_packet(uint8_t *buffer);
void parse_packet(uint8_t *input, int16_t *output);

uint8_t SendRecv_Byte(uint8_t byte);

void nrfInit();

// nRF
#define NRF_CE_PIN GPIO_PIN_1
#define NRF_CSN_PIN GPIO_PIN_2
#define NRF_IRQ_PIN GPIO_PIN_3

// chip select
#define NRF_SELECT GPIO_PORTE_DATA_R &= ~NRF_CSN_PIN
#define NRF_DESELECT GPIO_PORTE_DATA_R |= NRF_CSN_PIN

// data/control
#define NRF_CE_CMD GPIO_PORTE_DATA_R &= ~NRF_CE_PIN
#define NRF_CE_DATA GPIO_PORTE_DATA_R |= NRF_CE_PIN

#endif /* RADIO_H_ */
