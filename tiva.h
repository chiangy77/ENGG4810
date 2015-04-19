#ifndef TIVA_H_
#define TIVA_H_

#include "typedefs.h"

void initTiva();
void writeData(u_char data);
void writeCommand(u_char command);
void delay(u_char x10ms);

#define COMMAND 0
#define DATA 1

// clock
#define LCD_SCLK_PIN GPIO_PIN_4

// data out
#define LCD_MOSI_PIN GPIO_PIN_7

// chip select
#define LCD_CS_PIN GPIO_PIN_4
#define LCD_SELECT GPIO_PORTA_DATA_R &= ~LCD_CS_PIN
#define LCD_DESELECT GPIO_PORTA_DATA_R |= LCD_CS_PIN

// data/control
#define LCD_DC_PIN GPIO_PIN_4
#define LCD_DC_CMD GPIO_PORTE_DATA_R &= ~LCD_DC_PIN
#define LCD_DC_DATA GPIO_PORTE_DATA_R |= LCD_DC_PIN

#endif /* TIVA_H_ */

