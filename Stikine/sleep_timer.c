/*
 * sleep_timer.c
 *
 *  Created on: 11/20/2015
 *      Author: KDolgikh
 */

#include <MSP430.h>
#include <stdint.h>
#include "pinDefs.h"

extern uint8_t Break_Sleep;

void Sleep_Timer(uint8_t rollovers, uint16_t cycles) {
	uint8_t i;
	TA0CCR1 = TA0R + cycles; // num_clk changes to TA0CCR1 value at every call
	TA0CCR1 &= ~CCIFG;
	TA0CCR1 |= CCIE;
	LPM3;
	for (i = 0; i < rollovers; i++) {
		if (Break_Sleep) {
			Break_Sleep = False;
			break;
		}
		LPM3;
	}
	TA0CCR1 &= ~CCIE;
}
