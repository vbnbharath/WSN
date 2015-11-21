/*
 * sleep_timer.c
 *
 *  Created on: 11/20/2015
 *      Author: KDolgikh
 */

# include <MSP430.h>

void sleep_timer (unsigned long int num_ov, unsigned int num_clk)
{
	unsigned long int i;
	TA0CCR1 = TA0R + num_clk; // num_clk changes to TA0CCR1 value at every call
	LPM3;
	for (i = 0; i < num_ov; i++)
		LPM3;
}
