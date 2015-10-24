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

/**
 * \brief Main control sequence for sensor node
 * @return Constant 0, but it has nowhere to go.
 */
int main(void)
{
// Value line inits
	volatile uint8_t value;
	volatile uint8_t status[2];

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

#ifdef __MSP430FR5739__
#define Slow_Timer_Vector_0 TIMER0_A0_VECTOR
#define Fast_Timer_Vector_0 TIMER1_A0_VECTOR
#define GDO_Pin_Vector PORT1_VECTOR
#endif




/**
 * \brief Interrupt service routine for slow timer
 */
void __attribute__((__interrupt__(Slow_Timer_Vector_0)))TimerA_0_ISR(void)
{
	TACCTL0 &= ~CCIFG; // Clear the interrupt flag
	LED1Reg ^= LED1;
}

/**
 *  \brief Interrupt service routine for fast timer.
 */
void __attribute__((__interrupt__(Fast_Timer_Vector_0)))TimerA_1_ISR(void)
{
	TA1CCTL0 &= ~CCIFG; // Clear the interrupt flag
}

/**
 * \brief Interrupt service routine for packet received pin from CC100l
 */
void __attribute__((__interrupt__(GDO_Pin_Vector)))MSP_RX_ISR(void)
{

}

