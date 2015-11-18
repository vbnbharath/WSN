/*
 * sleep_timer.c
 *
 *  Created on: 17 но€б. 2015 г.
 *      Author: Admin
 */


static void TA0_init (void);
static void TA0_halt (void);


// Timer0_A is used to count us
void TA0_init (void)
{
	TA0CTL |= TASSEL_1; // TA clock source is ACLK, input divider 1
	TA0CCTL0 |= CCIE; // enable interrupt for TA0CCR0
	//TA0CTL |= TAIE;
}

void TA0_halt (void)
{
	TA0CCR0 = 0; // TA0 is halted, applicable to up mode
}

void sleep_us (unsigned int sleep_time) // sleep_time values: 1 to 65535; real time values: divide by 32768
{
	TA0CCR0 = sleep_time; // assign TA0CCR0 a sleep_time value
	TA0CTL |= MC_1; // TA is enabled, up mode
	LPM0;
	TA0_halt();
}


