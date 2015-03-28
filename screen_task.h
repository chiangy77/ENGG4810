/*
 * screen_task.h
 *
 *  Created on: Mar 25, 2015
 *      Author: Jordi
 */

#ifndef SCREEN_TASK_H_
#define SCREEN_TASK_H_

//
// Prototypes for the LED task.
//
//*****************************************************************************
extern uint32_t ScreenTaskInit(void);

//*****************************************************************************
//
// The item size and queue size for the LED message queue.
//
//*****************************************************************************
#define DIGITAL 0
#define ANALOG 1


#endif /* SCREEN_TASK_H_ */
