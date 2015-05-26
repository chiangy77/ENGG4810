#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

#include "filtering.h"

float convolution(float *pInputMatrix) {
	float result;
	uint8_t i;

	for (i = 0; i < 10; i++) {
		result = pInputMatrix[i] * WINDOW;
	}

	return result;
}

void shift_values(int newNumber) {
	uint8_t i;

	for(i = 9; i > 1; i--) {
		//matrix[i-1] = matrix[i]
	}
	//matrix[0] = newNumber
}
