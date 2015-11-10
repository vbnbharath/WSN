/*
 * Radio_LBT.c
 *
 *  Created on: Oct 22, 2015
 *      Author: cgoss
 */

#include "SPI_Library.h"
#include "CC110l.h"
#include "stdint.h"
#include "Radio_LBT.h"
#include "msp430.h"

LBT_Status LBT_Send(uint8_t address, uint8_t* message, uint8_t length)
{
	uint8_t status;
	uint8_t state;
	uint8_t FIFO_Space;
	uint8_t Old_GDO;
	LBT_Status return_status;

	// Configure GDO pin
	SPI_Read(GDO_RX, &Old_GDO);			// Capture and save the old setting for the GDO pin
	status = SPI_Send(GDO_RX, 0x06);	// Set the GDO to assert on preamble start and TX complete
	state = status & State_Bits;		// Mask off the state bits from the status byte

	if(state == SB_TX) // If a TX was already in progress.
	{
		return_status = Already_Transmitting;
		goto Cleanup;		// Yes. A Goddamn goto.
	}

	// Change to recieve mode to check for clear channel
	SPI_Strobe(SRX, Get_TX_FIFO); // Listen before transmit
	__delay_cycles(1000);
	SPI_Read_Status(PKTSTATUS, &status);	// Get packet status

	if(status & BIT6) // Check for carrier sense. If true then a carrier signal was recv'd and a collision occured.
	{
		return_status = Channel_Busy;	// shit!
		goto Cleanup;
	}

	SPI_Strobe(SIDLE, Get_TX_FIFO);		// stop the radio
	SPI_Send(TXFIFO, length + 1);		// Put in the length byte
	SPI_Send(TXFIFO, address);			// Put in the address after the length and before the payload

	status = SPI_Send_Burst(TXFIFO, message, length);	// Load the TX fifo
	FIFO_Space = status & FIFO_Bytes;		// Get the space left in the FIFO

	if(FIFO_Space == 0)	// FIFO space remaining of 0 means overflow
	{
		return_status = TX_Buffer_Overflow;
		goto Cleanup;
	}

	// GDO pin is set to pull Hi when packet TX starts, and LO after it's done.
	// Starting the TX and then enabling the interrupt catches only the second transition
	MSP_RX_Port_IFG	&= ~MSP_RX_Pin; // Clear existing interrupt flags
	MSP_RX_Port_IE |= MSP_RX_Pin;  	// Enable interrupts on the RX pin
	SPI_Strobe(STX, Get_TX_FIFO); // Tell radio to transmit

	LPM3; // Go to sleep while TX happens, GDO pin will wake MSP back up.
	MSP_RX_Port_IE &= ~MSP_RX_Pin;  	// Disable interrupts on the RX pi

	// Change to recieve mode to check for clear channel
	SPI_Strobe(SRX, Get_TX_FIFO); // Listen before transmit
	__delay_cycles(1000);
	SPI_Read_Status(PKTSTATUS, &status);	// Get packet status

	if(status & BIT6) // Check for carrier sense. If true then a carrier signal was recv'd and a collision occured.
	{
		return_status = TX_Collision;	// shit!
		goto Cleanup;
	}
	else
	{
		return_status = Transmit_Success;
	}



	// Clear the interrupt and set the GDO pin back to its old function here. n
	Cleanup:

	MSP_RX_Port_IE &= ~MSP_RX_Pin;  	// Disable interrupts on the RX pin
	SPI_Send(GDO_RX, Old_GDO);			// Set the GDO pin back to its old function

	return return_status;
}
