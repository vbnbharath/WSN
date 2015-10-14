/*
 * pinDefs.h
 *
 *  Created on: Oct 14, 2015
 *      Author: cgoss
 */

#ifndef PINDEFS_H_
#define PINDEFS_H_

// Value line pindefs
#ifdef __MSP430G2553
// Registers
#define LED1Reg P1OUT
#define LED2Reg P1OUT

// Pins
#define LED1 BIT0
#define LED2 BIT6
#endif


#endif /* PINDEFS_H_ */
