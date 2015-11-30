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
#include "Temperature_Measurement.h"

typedef enum {
	Waiting_For_Start,
	Localizing,
	Clustering,
	CH_TDMA_Assignment,
	CH_Sensing,
	TDMA_Assignment,
	Sensing
} Machine_State;
Machine_State state;

typedef enum {NODE, CLUSTER_HEAD} nd;
nd node = CLUSTER_HEAD;
//nd node = NODE;

// Globals
volatile uint16_t Timer_Rollover_Count = 0;
volatile uint8_t Break_Sleep = False;
uint8_t message[3];
uint8_t length = 3;
uint8_t rollovers_tdma;
uint16_t cycles_tdma;

typedef enum {
	NODE1=1, NODE2, NODE3, NODE4, NODE5, NODE6, NODE7, NODE8, NODE9, NODE10,
	NODE11, NODE12, NODE13, NODE14, NODE15, NODE16, NODE17, NODE18, NODE19, NODE20,
	NODE21, NODE22, NODE23, NODE24, NODE25, NODE26, NODE27, NODE28, NODE29, NODE30} nd_addr;
nd_addr node_address;

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

	if (node == CLUSTER_HEAD)
		state = CH_TDMA_Assignment;
	else if (node == NODE)
		state = TDMA_Assignment;

	while(1)
	{
		if (state == CH_TDMA_Assignment) {
			rollovers_tdma=0;
			cycles_tdma=32441; // defines a sleep period of 990 ms
			message[0]=rollovers_tdma;
			int_divide (cycles_tdma, &message[1]);
			uint8_t i = 0;
			for (i=1; i<11; i++) {
				node_address=i;
				//TDMA_Send((node_address = i), NODE11, message, length);
			}
		}
		if (state == TDMA_Assignment)
			;
	}
}


#ifdef __MSP430G2553__
#define Slow_Timer_Vector_1 TIMER0_A1_VECTOR
#define GDO_Pin_Vector PORT1_VECTOR
#define ADC_Vector ADC10_VECTOR
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

void __attribute__((__interrupt__(ADC_Vector)))ADC_ISR(void)
{
	ADC10CTL0 &= ~ADC10IFG; // Clear flag
	LPM0_EXIT;

}

