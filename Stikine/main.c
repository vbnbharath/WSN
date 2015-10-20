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
#include <SPI_Pins.h>

#define transmitter

volatile uint8_t statusVal[10];

/**
 * \brief Main control sequence for sensor node
 * @return Constant 0, but it has nowhere to go.
 */
int main(void)
{
// Value line inits

	Board_Init();
	Timer_Init();
	SPI_Init(); // Start SPI
	Radio_Init(); // Prep the radio

#ifdef receiver

	SPI_Strobe(SRX, Get_RX_FIFO);// Clear RX FIFO
	__bis_SR_register(LPM3_bits + GIE);

    return 0; // Never get here
#endif

#ifdef transmitter

    __bis_SR_register(LPM3_bits + GIE); // Go to sleep
    return 0;
#endif
}



/**
 * \brief Interrupt service routine for slow timer
 */
void __attribute__((__interrupt__(TIMER0_A0_VECTOR)))TimerA_0_ISR(void)
{
	TACCTL0 &= ~CCIFG; // Clear the interrupt flag
	LPM3_EXIT; // Wake up!
}

/**
 *  \brief Interrupt service routine for fast timer.
 */
void __attribute__((__interrupt__(TIMER1_A0_VECTOR)))TimerA_1_ISR(void)
{
	static int i = 0;

	TA1CCTL0 &= ~CCIFG; // Clear the interrupt flag

	if(i >= 20)
	{
		LED2Reg ^= LED2;
		i=0;
	}
	else
	{
		i++;
	}
}

/**
 * \brief Interrupt service routine for packet received pin from CC100l
 */
void __attribute__((__interrupt__(MSP_RX_Port_IV)))MSP_RX_ISR(void)
{
    volatile uint8_t testVal[64];
	volatile uint8_t bytes_in_FIFO;
	volatile uint8_t rBytesVal;

	MSP_RX_Port_IFG &= ~MSP_RX_Pin;

	rBytesVal = SPI_Strobe(SNOP, Get_RX_FIFO);
	bytes_in_FIFO = rBytesVal & FIFO_Bytes;

	statusVal[2] = SPI_Read_Burst(RXFIFO, testVal, bytes_in_FIFO);
	SPI_Strobe(SIDLE, Get_RX_FIFO); // start RX again
	__delay_cycles(1000000);
	SPI_Strobe(SRX, Get_RX_FIFO); // start RX again

	P1OUT ^= BIT0;
}

/**
 * \brief Interrupt service routine for pushbutton on the MSP430
 */

void __attribute__((__interrupt__(PORT1_VECTOR)))Button_ISR(void)
{
	uint8_t pinState;
	volatile uint8_t status;
	P1IFG &= ~BIT3; // Clear interrupt flag

	pinState = P1IN & BIT3;
	__delay_cycles(100);

	if(pinState == (P1IN & BIT3))
		{
			SPI_Send(TXFIFO, 0x01);
			SPI_Send(TXFIFO, 0xFF);
			status = SPI_Strobe(STX, Get_TX_FIFO);
		}
}
