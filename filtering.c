#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/ssi.h"
#include <driverlib/sysctl.h>
#include "drivers/rgb.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"
#include "typedefs.h"
#include <math.h>

#include "filtering.h"

int16_t convolution(int16_t pInputMatrix[5], uint8_t flag) {
	float dummy;
	uint8_t i;
	int32_t result;

	if (flag > 4) {
		for (i = 0; i < 5; i++) {
			dummy += ((float) pInputMatrix[i]) * WINDOW;
		}
	}

	result = (int16_t) dummy;
	return result;
}


int16_t median(int16_t pMedianMatrix[3], uint8_t flag) {
	uint16_t middle = 0;

	if (flag > 1) {
		if ((pMedianMatrix[0] <= pMedianMatrix[1]) && (pMedianMatrix[0] <= pMedianMatrix[2])) {
			middle = (pMedianMatrix[1] <= pMedianMatrix[2]) ? pMedianMatrix[1] : pMedianMatrix[2];
		} else if ((pMedianMatrix[1] <= pMedianMatrix[0]) && (pMedianMatrix[1] <= pMedianMatrix[2])) {
			middle = (pMedianMatrix[0] <= pMedianMatrix[2]) ? pMedianMatrix[0] : pMedianMatrix[2];
		} else {
			middle = (pMedianMatrix[0] <= pMedianMatrix[1]) ? pMedianMatrix[0] : pMedianMatrix[1];
		}
	}

	return middle;
}

int16_t medianOf5Values(int16_t medianArray[5], uint8_t flag) {

	uint8_t i, j;
	int16_t temp;

	if (flag > 4) {
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4-i; j++) {
				if (medianArray[j] > medianArray[j+1]) {
					temp = medianArray[j+1];
					medianArray[j+1] = medianArray[j];
					medianArray[j] = temp;
				}
			}
		}
	}
	return medianArray[2];
}

