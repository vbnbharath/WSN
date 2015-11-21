/**
 * \file main.c
 * \brief Main control code and interrupt definitions for sensor node
 *
 * Initializes both the MSP430 controller and the attached CC110l radio.
 * After completing the initialization process the controller goes into LPM3 and waits.
 */

#include <msp430.h> // Base header files
#include <stdint.h> // pull in standard datatypes
#include <pinDefs.h> // Pin defines
#include <CC110l.h> // Literals for helping with the radio
#include <MSP_Init.h> // Code to set initial board state
#include <SPI_Library.h> // SPI control for the radio
#include "sleep_timer.h" // Sleep code for controller

// Globals
volatile uint16_t Timer_Rollover_Count = 0;

/**
 * \brief Main control sequence for sensor node
 * @return Constant 0, but it has nowhere to go.
 */
int main(void)
{
	Board_Init();
	Timer_Init();
	SPI_Init(); // Start SPI
	Radio_Init(); // Prep the radio

	__bis_SR_register(LPM3_bits + GIE);
    return 0; // Never get here
}


#ifdef __MSP430G2553__
#define Slow_Timer_Vector_0 TIMER0_A0_VECTOR
#define Fast_Timer_Vector_0 TIMER1_A0_VECTOR
#define GDO_Pin_Vector PORT1_VECTOR
#endif

/**
 * \brief Interrupt service routine for slow timer
 */
void __attribute__((__interrupt__(Slow_Timer_Vector_0)))TimerA_0_ISR(void)
{
	switch (TA0IV) {
		case(TA0IV_TAIFG):
			Timer_Rollover_Count++;
			break;
		case(TA0IV_TACCR1):
			LPM3_EXIT; // Wake up if CCR1 is hit, used for sleeping function
			break;
		case(TA0IV_TACCR2):
			break;
		default: break;
	}
}

/**
 *  \brief Interrupt service routine for fast timer.
 */
void __attribute__((__interrupt__(Fast_Timer_Vector_0)))TimerA_1_ISR(void)
{

}

/**
 * \brief Interrupt service routine for packet received pin from CC100l
 */
void __attribute__((__interrupt__(GDO_Pin_Vector)))MSP_RX_ISR(void)
{

}

