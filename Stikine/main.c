/**
 * \file main.c
 * \brief Main control code and interrupt definitions for sensor node
 *
 * Initializes both the MSP430 controller and the attached CC110l radio.
 * After completing the initialization process the controller goes into LPM3 and waits.
 */

#include <msp430.h> // Base header files
#include <stdint.h> // pull in standard datatypes
#include "pinDefs.h" // Pin defines
#include "CC110l.h" // Literals for helping with the radio
#include "MSP_Init.h" // Code to set initial board state
#include "SPI_Library.h" // SPI control for the radio
#include "Radio_LBT.h"

uint8_t message[50];

/**
 * \brief Main control sequence for sensor node
 * @return Constant 0, but it has nowhere to go.
 */
int main(void) {
	Board_Init();
	Timer_Init();
	SPI_Init(); // Start SPI
	Radio_Init(); // Prep the radio

	int i = 0;
	while (i < 50) {
		message[i] = i;
		i++;
	}

	while (1) {
		LBT_Send(0xFF, 0xEB, message, 50);
		LPM3;
	}

	return 0;
}

#ifdef __MSP430G2553__
#define Slow_Timer_Vector_0 TIMER0_A0_VECTOR
#define Fast_Timer_Vector_0 TIMER1_A0_VECTOR
#define GDO_Pin_Vector PORT1_VECTOR
#endif

/**
 * \brief Interrupt service routine for slow timer
 */
void __attribute__((__interrupt__(Slow_Timer_Vector_0))) TimerA_0_ISR(void) {
	TACCTL0 &= ~CCIFG; // Clear the interrupt flag
	LPM3_EXIT;
}

/**
 *  \brief Interrupt service routine for fast timer.
 */
void __attribute__((__interrupt__(Fast_Timer_Vector_0))) TimerA_1_ISR(void) {
	TA1CCTL0 &= ~CCIFG; // Clear the interrupt flag
}

/**
 * \brief Interrupt service routine for packet received pin from CC100l
 */
void __attribute__((__interrupt__(GDO_Pin_Vector))) MSP_RX_ISR(void) {
	MSP_RX_Port_IFG &= ~MSP_RX_Pin; // Clear the interrupt flag
	LPM3_EXIT; // Wake up on interrupt
}

