/*
 * Radio_LBT.c
 *
 *  Created on: Oct 22, 2015
 *      Author: cgoss
 */

#include<SPI_Library.h>
#include<CC110l.h>
#include<stdint.h>
#include<Radio_LBT.h>
#include<msp430.h>

LBT_Status LBT_Send(uint16_t address, uint8_t* message, uint8_t length)
{
	volatile uint8_t status;
	volatile uint8_t state;
	volatile uint8_t FIFO_Space;

	status = SPI_Strobe(SNOP, Get_TX_FIFO);
	state = status & State_Bits;

	if(state != SB_Idle)
	{
		return Radio_Not_Idle;
	}

	// Load the TX fifo
	status = SPI_Send_Burst(TXFIFO, message, length);
	FIFO_Space = status & FIFO_Bytes;

	if(FIFO_Space == 0)
	{
		return TX_Buffer_Overflow;
	}

	SPI_Strobe(STX, Get_TX_FIFO); // Tell radio to transmit
	LPM3; // Go to sleep while TX happens, GDO pin will wake MSP back up.
	SPI_Strobe(SRX, Get_TX_FIFO); // Listen after transmit

	return Transmit_Success;

}
