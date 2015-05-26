/*
 * filtering.h
 *
 *  Created on: 26 May 2015
 *      Author: Matt
 */

#ifndef FILTERING_H_
#define FILTERING_H_

#define WINDOW 0.1 //assuming flat window at 10 samples

float convolution(float *pInputMatrix);
void shift_values(int newNumber);

//float window[10];
//int window[10] = [1,1,1,1,1,1,1,1,1,1];

#endif /* FILTERING_H_ */
