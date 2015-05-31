/*
 * screen_task.h
 *
 *  Created on: Mar 25, 2015
 *      Author: Jordi
 */

#ifndef SCREEN_TASK_H_
#define SCREEN_TASK_H_

extern int state;
//extern int diameter;
extern uint32_t ScreenTaskInit(void);

void changeDate (int year, int month, int day, int cursor);
uint8_t DCB(int input);
void displayTimeDate (int year, int month, int day, int hour, int minute, int second);

#define DIGITAL 0
#define ANALOG 1
#define MAIN_MENU 	0
#define ALG_DGT 	1
#define FROM_RTC	2
#define TIM_DAT 	3
#define PARAMS 		4
#define STOP 		5
#define BREAK 		6
#define CHANGE_TIME 7
#define CHANGE_DIAM 8
#define CHANGE_MASS 9
#define CHANGE_DATE 10

#define PI 3.1416

#define SSI_FRF_MOTO_MODE_0     0x00000000
#define SSI_MODE_MASTER         0x00000000

struct packet {
	uint8_t speed;
	int8_t accel;
	int16_t odom;
};

#endif /* SCREEN_TASK_H_ */
