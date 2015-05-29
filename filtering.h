/*
 * filtering.h
 *
 *  Created on: 26 May 2015
 *      Author: Matt
 */

#ifndef FILTERING_H_
#define FILTERING_H_

#define WINDOW  0.2 //assuming flat window at 5 samples

int16_t convolution(int16_t pInputMatrix[5], uint8_t flag);
int16_t median(int16_t pMedianMatrix[3], uint8_t flag);
int16_t medianOf5Values(int16_t medianArray[5], uint8_t flag);

//float window[10];
//int window[10] = [1,1,1,1,1,1,1,1,1,1];

#endif /* FILTERING_H_ */
