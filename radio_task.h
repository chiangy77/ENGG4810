/*
 * radio_task.h
 *
 *  Created on: 6 May 2015
 *      Author: Matt
 */

#ifndef RADIO_TASK_H_
#define RADIO_TASK_H_

extern uint32_t RadioTaskInit(void);

// nRF
#define NRF_CE_PIN GPIO_PIN_1
#define NRF_CSN_PIN GPIO_PIN_2
#define NRF_IRQ_PIN GPIO_PIN_3


#define SSI_FRF_MOTO_MODE_0     0x00000000  // Moto fmt, polarity 0, phase 0
#define SSI_MODE_MASTER         0x00000000  // SSI master

#endif /* RADIO_TASK_H_ */
