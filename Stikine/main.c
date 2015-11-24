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
#include "Radio.h"
#include "SPI_Library.h" // SPI control for the radio
#include "sleep_timer.h" // Sleep code for controller

typedef enum {
	Waiting_For_Start,
	Localizing,
	Clustering,
	CH_TDMA_Assignment,
	CH_Sensing,
	TDMA_Assignment,
	Sensing
} Machine_State;

// Globals
volatile uint16_t Timer_Rollover_Count = 0;
volatile uint8_t Break_Sleep = False;
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
	LBT_Status status;

	uint8_t i;
	for (i = 0; i < 50; i++) {
		message[i] = i;
	}

	while (True) {
		Sleep_Timer(0, 2);
		status = LBT_Send(0xFF, 0xAA, message, 50);

		if(status == Transmit_Success)
		{
			LED1Reg ^= LED1;
		}
	}
}

#ifdef __MSP430G2553__
#define Slow_Timer_Vector_1 TIMER0_A1_VECTOR
#define GDO_Pin_Vector PORT1_VECTOR
#endif

/**
 * \brief Interrupt service routine for slow timer
 */
void __attribute__((__interrupt__(Slow_Timer_Vector_1))) TimerA_0_ISR(void) {
	switch (TA0IV) {
	case (TA0IV_TAIFG):
		Timer_Rollover_Count++;
		break;
	case (TA0IV_TACCR1):
		LPM3_EXIT; // Wake up if CCR1 is hit, used for sleeping function
		break;
	case (TA0IV_TACCR2):
		break;
	default:
		break;
	}
}

/**
 * \brief Interrupt service routine for packet received pin from CC100l
 */
void __attribute__((__interrupt__(GDO_Pin_Vector))) MSP_RX_ISR(void) {
	MSP_RX_Port_IFG &= ~MSP_RX_Pin; // Clear the interrupt flag
	Break_Sleep = True;
	LPM3_EXIT; // Wake up on interrupt
}

