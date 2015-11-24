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
#include "states.h" // Enum for the possible machine states.

// Globals
volatile uint16_t Timer_Rollover_Count = 0;
volatile uint8_t Break_Sleep = False;
Machine_State state = Waiting_For_Start;

/**
 * \brief Main control sequence for sensor node
 * @return Constant 0, but it has nowhere to go.
 */

int main(void)
{
	volatile uint16_t timeout;
	//volatile struct LBT_ST C;
	uint8_t i;
	Board_Init();
	Timer_Init();
	SPI_Init(); // Start SPI
	Radio_Init(); // Prep the radio


	for(i=0;i<16;i++)
	{
		status = LBT_Send(0xF0, 0xA0, &message, 1);
		If(status == Transmit_Success)
		{
			Break;
		}
		//LBT_Listen()
	}

	message = 0;
	return 0;

	while(True)
	{
		if(state == Waiting_For_Start)
		{
			state = Wait_For_Start();
		}

		if(state == Localizing)
		{
			while(True)
			{
				// Spin here.
			}
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
void __attribute__((__interrupt__(Slow_Timer_Vector_1)))TimerA_0_ISR(void)
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
 * \brief Interrupt service routine for packet received pin from CC100l
 */
void __attribute__((__interrupt__(GDO_Pin_Vector)))MSP_RX_ISR(void)
{
	MSP_RX_Port_IFG &= ~MSP_RX_Pin; // Clear the interrupt flag
	Break_Sleep = True;
	LPM3_EXIT; // Wake up on interrupt
}

