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
typedef enum{
	Listen,
	ChangeChannel,
	Readout_Packet
}State_T;

State_T state;
uint8_t Chan;


int main(void)
{
// Value line inits
	volatile uint8_t value;
	volatile uint8_t status[2];

	Board_Init();
	Timer_Init();
	SPI_Init(); // Start SPI
	Radio_Init(); // Prep the radio


	MSP_RX_Port_IE |= MSP_RX_Pin;
	MSP_RX_Port_IFG &= ~MSP_RX_Pin;

	SPI_Send(GDO_RX, 0x0E);
	SPI_Send(ADDR, 0xF0);		//Set Address of Radio
	SPI_Send(PKTCTRL1,0x01);	//Set to do address filter with two broadcasts
	SPI_Strobe(SRX, Get_RX_FIFO);
	state = Listen;			//Set initial state to Sweep through channels

	SPI_Send(CHANNR,0);					//Tell radio what channel to use
	//SPI_Strobe(SRX,Get_RX_FIFO);

	while(1)
	{

		if(state == ChangeChannel)//Change the channel by going to idle, set channel, enter receive then listen for carrier on channel in sleep mode
		{

			SPI_Strobe(SIDLE, Get_RX_FIFO);			//Tell radio to enter idle mode so the channel can be changed
			SPI_Send(GDO_RX,0x0E);					//Tell radio to trigger interrupt on carrier sense

//			if(Chan > 9)
//			{
//				Chan = 0;								//Reset to channel zero
//			}

			SPI_Send(CHANNR,Chan);					//Tell radio what channel to use
			SPI_Strobe(SRX,Get_RX_FIFO);		//Tell radio to enter receive mode
			__delay_cycles(10000);				//Wait while using lots of power (Replace with low power mode + ISR later)
			Chan++;

		}

		if(state == Listen)  //If carrier found, enter new state, listen to channel for longer time for packet, if no packet found, enter change mode
		{
			__bis_SR_register(LPM3_bits + GIE);
			//__delay_cycles(100000);  //Delay longer than before, (Replace with timer and sleep mode later)

			//state = ChangeChannel;		//Change state back to channel sweep, no packet was found on this channel afterall.
		}
	}



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
	TACCTL0 &= ~CCIFG; // Clear the interrupt flag


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
	MSP_RX_Port_IFG &= ~MSP_RX_Pin;

	if(state == Listen)
	{
		volatile uint8_t length;		//Place to store length of data
		volatile uint8_t Data[64];		//Place to store data


		SPI_Read_Status(RXBYTES, &length);		//Read the length of the stuff in RXFIFO
		SPI_Read_Burst(RXFIFO, Data, length);		//Read data in RXFIFO
		SPI_Read(CHANNR, &Chan);		//Return the channel that had the packet
		Data[0] = 0;					//Place to place a breakpoint?
	}


	if(state == ChangeChannel)
	{

		state = Listen;			//Set State to listen when this part is over
		SPI_Send(GDO_RX,0x07);	//Set Pin to go high on packet with CRC ok received.

	}


}

