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
#include "UART.h"

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
uint8_t Address = 0x55;
/**
 * \brief Main control sequence for sensor node
 * @return Constant 0, but it has nowhere to go.
 */
int main(void) {
	Board_Init();	//Start the board
	Timer_Init();	//Ready the timers
	SPI_Init(); // Start SPI
	Radio_Init(); // Prep the radio
	UART_Init(); //Set up the UART communication

	MSP_RX_Port_IE |= MSP_RX_Pin;		//Enable the interrupt
	MSP_RX_Port_IFG &= ~MSP_RX_Pin;	//Clear the interrupt flag so it does not trigger immediately.
	SPI_Strobe(SFRX, Get_RX_FIFO);	//Flush the RX_FIFO
	SPI_Send(GDO_RX, 0x07 | BIT6);//Set the radio into receive mode, set the triggering from high to low instead of low to high.
	SPI_Send(ADDR, Address);		//Set Address of Radio
	SPI_Send(PKTCTRL1, 0x07);	//Set to do address filter with broadcasts
	SPI_Strobe(SRX, Get_RX_FIFO);  //Set to recieve mode

	SPI_Send(CHANNR, 0);					//Tell radio what channel to use
	__bis_SR_register(LPM3_bits + GIE);	//Send into sleep mode (Wont Need After addition of sleep mode function for specific time.

	return 0; // Never get here

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

	uint8_t Chan;
	volatile uint8_t length;		//Place to store length of data
	volatile uint8_t Data[64];		//Place to store data
	volatile uint8_t Message[256];
	uint8_t NumBuff[5];
	SPI_Read_Status(RXBYTES, &length);	//Read the length of the stuff in RXFIFO
	SPI_Read_Burst(RXFIFO, Data, length);		//Read data in RXFIFO
	SPI_Read(CHANNR, &Chan);		//Return the channel that had the packet
	uint8_t Mpos = 0;
	Message[Mpos++] = 'C';	//This section is all formatting the data for UART
	Message[Mpos++] = ':';
	Message[Mpos++] = ' ';

	Convert_to_Decimal(Chan, NumBuff);

	int i = 0;
	while (i < 3) {
		Message[3 + i] = NumBuff[i];
		i++;
		Mpos++;
	}

	Message[Mpos++] = ' ';
	Message[Mpos++] = 'D';
	Message[Mpos++] = ':';
	Message[Mpos++] = ' ';

	Convert_to_Decimal(Data[1], NumBuff);
	i = 0;
	while (i < 3) {
		Message[Mpos] = NumBuff[i];
		i++;
		Mpos++;
	}

	Message[Mpos++] = ' ';
	Message[Mpos++] = 'S';
	Message[Mpos++] = ':';
	Message[Mpos++] = ' ';

	Convert_to_Decimal(Data[2], NumBuff);
	i = 0;
	while (i < 3) {
		Message[Mpos] = NumBuff[i];
		i++;
		Mpos++;
	}
	Message[Mpos++] = ' ';
	Message[Mpos++] = 'R';
	Message[Mpos++] = ':';
	Message[Mpos++] = ' ';

	Convert_to_Decimal(Data[length - 2], NumBuff);

	i = 0;
	while (i < 3) {
		Message[Mpos] = NumBuff[i];
		i++;
		Mpos++;
	}

	Message[Mpos++] = ' ';
	Message[Mpos++] = '>';
	Message[Mpos++] = '>';
	Message[Mpos++] = '>';
	Message[Mpos++] = ' ';

	Convert_to_Hex(&Message[Mpos++], &Data[3], length - 6);

	UARTSendArray(Message);
	SPI_Strobe(SRX, Get_RX_FIFO);
}

void __attribute__((__interrupt__(USCIAB0RX_VECTOR))) USCI0RX_ISR(void) {

	static uint8_t Command[7];		//Place to store temporary number/commands
	static uint8_t Pointer = 0;	//Pointer stores how many digits were entered
	static uint8_t Buff;			//Place to store RXBuffer data
	uint8_t Channel = 0;				//Channel Selection
	uint8_t Number = 0;				//Just intermediate number
	Buff = UCA0RXBUF;				//Set Buff to value in the buffer
	int i;

	if (Buff == 13) {

		if (Command[0] == 'B') {
			uint8_t Message = 0xFF;
			LBT_Send(0xFF, Address, &Message, 1);
			UARTSendArray("b");

		}

		else {
			UARTSendArray("Channel");
			for (i = 0; i < Pointer; i++) {
				Number = Command[i] - '0';	//Set command to number in decimal
				Channel = Channel * 10 + Number;//Set the channel on digit at a time
			}
			SPI_Strobe(SIDLE, Get_RX_FIFO);			//Put radio idle mode to:
			SPI_Send(CHANNR, Channel);			//Change frequency after in idle
			SPI_Strobe(SRX, Get_RX_FIFO);			//Put back into receive mode
			Pointer = 0;
		}
	}

	else {
		Command[Pointer++] = Buff;
	}

}

