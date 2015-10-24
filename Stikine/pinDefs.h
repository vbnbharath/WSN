/*
 * pinDefs.h
 *
 *  Created on: Oct 14, 2015
 *      Author: cgoss
 */

#ifndef PINDEFS_H_
#define PINDEFS_H_


/*************************** Begin Value line pindefs ******************************/
#ifdef __MSP430G2553__
// Registers
#define LED1Reg P1OUT
#define LED2Reg P1OUT

// Pins
#define LED1 BIT0
#define LED2 BIT6

#endif				// Value line pindefs end


#ifdef __MSP430FR5739__
#define LED1Reg PJOUT
#define LED2Reg PJOUT

#define LED1Dir PJDIR
#define LED2Dir PJDIR


#define LED1 BIT1
#define LED2 BIT1
#endif				// Value line pindefs end


#endif /* PINDEFS_H_ */
