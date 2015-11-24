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
	TA0CCR1 = TA0R + cycles; // Set the CCR to wait the number of cycles given
	TA0CCR1 &= ~CCIFG;
	TA0CCR1 |= CCIE;
	LPM3;
	for (i = 0; i < rollovers && !Break_Sleep; i++) {	// Sleeps for i rollovers or until the Break_Sleep variable is set true
		LPM3;
	}

	Break_Sleep = False;
	TA0CCR1 &= ~CCIE;
}
