/*
 * sleep_timer.c
 *
 *  Created on: 11/20/2015
 *      Author: KDolgikh
 */

#include <MSP430.h>
#include <stdint.h>

void sleep_timer(uint8_t rollovers, uint16_t cycles) {
	uint8_t i;
	TA0CCR1 = TA0R + cycles; // num_clk changes to TA0CCR1 value at every call
	LPM3;
	for (i = 0; i < rollovers; i++) {
		LPM3;
	}
}
