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

typedef enum {CLUSTER1, CLUSTER2} cl;
cl cluster = CLUSTER1;
//nd cluster = CLUSTER2;

#define ROLLOVERS_POS 0
#define CYCLES_POS_LOWER 1
#define CYCLES_POS_UPPER 2

#define TDMA_SLOT 257 // defines time slot of 10 ms
#define TDMA_SLEEP 32441  // defines a sleep period of 990 ms
#define GUARD_TIME 66 // guard time of 65 cycles or 2 ms or 2014 us

// Globals
volatile uint16_t Timer_Rollover_Count = 0;
volatile uint8_t Break_Sleep = False;

uint8_t message[3];
uint8_t length = 3;
uint8_t rollovers_tdma;
uint16_t cycles_tdma;

struct Listen_Struct RX_Message;

typedef enum {
	NODE1=1, NODE2, NODE3, NODE4, NODE5, NODE6, NODE7, NODE8, NODE9, NODE10,
	NODE11} nd_addr_1;
nd_addr_1 node_address_cluster1;
// CH cluster 1 has address 11 or NODE11, cluster consist of 10 nodes

typedef enum {
	NODE12=12, NODE13, NODE14, NODE15, NODE16, NODE17, NODE18, NODE19, NODE20,
	NODE21} nd_addr_2;
nd_addr_2 node_address_cluster2;
// CH cluster 2 has address 21 or NODE21, cluster consist of 10 nodes

volatile uint16_t start; // number of cycles when TDMA assign starts
volatile uint16_t stop; // number of cycles when TDMA assign stops
volatile uint8_t ro_start_TDMA; // number of rollovers when TDMA assign starts
volatile uint8_t ro_stop_TDMA; // number of rollovers when TDMA assign stops
volatile uint16_t cycles_TDMA; // cycles spent during TDMA time slots dissemination

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
			uint8_t i;
			rollovers_tdma=0;
			cycles_tdma=TDMA_SLEEP;
			message[0]=rollovers_tdma;
			int_divide (cycles_tdma, &message[1]);
			if (cluster == CLUSTER1) {
				ro_start_TDMA = Timer_Rollover_Count;
				start = TA0R;
				for (i=1; i<11; i++) { // number of nodes is 10
					//start = TA0R;
					TDMA_Send((node_address_cluster1 = i), NODE11, &message[0], length);
					//stop = TA0R;
					Sleep_Timer(0,TDMA_SLOT); // makes interval of 10 ms
					//stop = TA0R;
				}
				stop = TA0R;
				ro_stop_TDMA = Timer_Rollover_Count;
				P1OUT ^= BIT0; // red // it is here just to set a breakpoint after stop
				// TDMA_Send lasts for 70 cycles (69 excluding stop)
				// (TDMA_send + sleep) last for 328 cycles or 10010 us (error less then resolution)
				// Sending data to 10 nodes lasts 3279-3280-3281 cycles with 3280 cycles occuring most often
				// 3280 cycles = 100 ms = 100097 us
			}
			else {
				ro_start_TDMA = Timer_Rollover_Count;
				start = TA0R;
				for (i=11; i<21; i++) { // number of nodes is 10
					TDMA_Send((node_address_cluster2 = i), NODE21, &message[0], length);
					Sleep_Timer(0,TDMA_SLOT); // makes interval of 10 ms
				}
				stop = TA0R;
				ro_stop_TDMA = Timer_Rollover_Count;
			}
			cycles_TDMA = cycles_spent(start, ro_start_TDMA, stop, ro_stop_TDMA);
			state == Sensing;
			Sleep_Timer(0,(TDMA_SLEEP - cycles_TDMA));

			//wake up an proceed to ch_sensing
			//do ch_sensing for 25 sec
			//repeat ch_tdma_assignment
		}
		if (state == TDMA_Assignment) {
			RX_Message = LBT_Listen(32768); // listen for 1 sec
			uint8_t rollovers = 0;
			uint16_t cycles = 0;
			rollovers = RX_Message.payload[ROLLOVERS_POS];
			cycles = int_merge(RX_Message.payload[CYCLES_POS_LOWER],RX_Message.payload[CYCLES_POS_UPPER]);

			//state == Sensing;
			Sleep_Timer(rollovers, (cycles - GUARD_TIME));
			// RX message, go to sleep
			// wake up for Sensing
			// do Sensing for 25 sec, then return to TDMA_Assignment
			//

		}

		if (state == Sensing) {

			//uint8_t i = 0;

			// loop to spin here for 25 sec with sleep
		}
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
	start = TA0R;
	MSP_RX_Port_IFG &= ~MSP_RX_Pin; // Clear the interrupt flag
	Break_Sleep = True;
	LPM3_EXIT; // Wake up on interrupt
}

void __attribute__((__interrupt__(ADC_Vector)))ADC_ISR(void)
{
	ADC10CTL0 &= ~ADC10IFG; // Clear flag
	LPM0_EXIT;

}

