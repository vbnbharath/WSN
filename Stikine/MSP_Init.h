/*
 * MSP_Value.h
 *
 *  Created on: Oct 19, 2015
 *      Author: cgoss
 */

#ifndef MSP_INIT_H_
#define MSP_INIT_H_


#ifdef __MSP430G2553__
#define SLOW_TIMER_CTL0 TACCTL0
#endif

#ifdef __MSP430FR5739__
#define SLOW_TIMER_CTL0 TA0CCTL0
#endif




void Board_Init(void);
void Timer_Init(void);
void Radio_Init(void);

#endif /* MSP_VALUE_H_ */
