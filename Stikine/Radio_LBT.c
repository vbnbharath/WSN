/*
 * Radio_LBT.c
 *
 *  Created on: Oct 22, 2015
 *      Author: Cody Gossel
 */

#include "SPI_Library.h"
#include "CC110l.h"
#include "stdint.h"
#include "Radio_LBT.h"
#include "msp430.h"

LBT_Status LBT_Send(uint8_t dest_address, uint8_t source_address, uint8_t *message, uint8_t length)
{
	uint8_t status;
	uint8_t state;
	uint8_t FIFO_Space;
	uint8_t Old_GDO;
	uint8_t Old_MSP_RX_Port_IES;
	uint8_t Old_MSP_RX_Port_IE;
	LBT_Status return_status;

	// Configure machine state
	SPI_Read(GDO_RX, &Old_GDO);			// Capture and save the old setting for the GDO pin
	Old_MSP_RX_Port_IES = MSP_RX_Port_IES;	// Save the old interrupt edge select value
	Old_MSP_RX_Port_IE = MSP_RX_Port_IE;	// Save the old interrupt enable value

	status = SPI_Send(GDO_RX, 0x06);	// Set the GDO to assert on preamble start and TX complete
	state = status & State_Bits;		// Mask off the state bits from the status byte

	if(state != SB_Idle) // If a TX was already in progress.
	{
		return_status = Radio_Busy;
		goto Cleanup;		// Yes. A Goddamn goto.
	}

	// Change to recieve mode to check for clear channel
	SPI_Strobe(SRX, Get_TX_FIFO); // Listen before transmit
	__delay_cycles(10000);
	SPI_Read_Status(PKTSTATUS, &status);	// Get packet status

	if(!(status & BIT4)) // Check for clear channel. If true then a carrier signal was recv'd and a collision occured.
	{
		return_status = Channel_Busy;	// shit!
		goto Cleanup;
	}

	SPI_Strobe(SIDLE, Get_TX_FIFO);		// stop the radio

	// Load the TX fifo
	SPI_Send(TXFIFO, length + 2);
	SPI_Send(TXFIFO, dest_address);
	SPI_Send(TXFIFO, source_address);
	status = SPI_Send_Burst(TXFIFO, message, length);	// Load the TX fifo
	FIFO_Space = status & FIFO_Bytes;		// Get the space left in the FIFO

	if(FIFO_Space == 0)	// FIFO space remaining of 0 means overflow
	{
		return_status = TX_Buffer_Overflow;
		goto Cleanup;
	}

	// Enable interrupt on falling edge

	MSP_RX_Port_IES |= MSP_RX_Pin;
	MSP_RX_Port_IFG &= ~MSP_RX_Pin;			// Clear interrupt flags
	MSP_RX_Port_IE |= MSP_RX_Pin;

	SPI_Strobe(STX, Get_TX_FIFO); // Tell radio to transmit
	LPM3; // Sleep until TX is done, interrupt will wake up the

	return_status = Transmit_Success;
	SPI_Strobe(SIDLE, Get_TX_FIFO); // Tell radio to idle

	// Clear the interrupt and set the GDO pin back to its old function here. n
	Cleanup:
	SPI_Send(GDO_RX, Old_GDO);				// Set the GDO pin back to its old function
	MSP_RX_Port_IES = Old_MSP_RX_Port_IES;	// Restore the old interrupt edge select value
	MSP_RX_Port_IE = Old_MSP_RX_Port_IE;	// Restore the old interrupt enable value

	return return_status;
}

LBT_Status LBT_Listen(uint16_t timeoutPeriod, uint8_t *out)
{
	uint8_t Old_GDO;
	uint8_t Old_MSP_RX_Port_IES;
	uint8_t Old_MSP_RX_Port_IE;
	uint8_t status;
	uint8_t state;
	LBT_Status return_status;

	// Flush the RX FIFO
	SPI_Strobe(SFRX, Get_RX_FIFO);

	// Configure machine state
	SPI_Read(GDO_RX, &Old_GDO);			// Capture and save the old setting for the GDO pin
	Old_MSP_RX_Port_IES = MSP_RX_Port_IES;	// Save the old interrupt edge select value
	Old_MSP_RX_Port_IE = MSP_RX_Port_IE;	// Save the old interrupt enable value


	status = SPI_Send(GDO_RX, 0x07);	// Set the GDO to assert on pkt recieve with crc OK
	state = status & State_Bits;		// Mask off the state bits from the status byte

	if(state != SB_Idle) // If a radio is already busy
	{
		return_status = Radio_Busy;
		goto Cleanup;		// Yes. A Goddamn goto.
	}

	// Set GDO pin to trigger on rising edge for PKT RX
	MSP_RX_Port_IES &= ~MSP_RX_Port_IE;
	MSP_RX_Port_IE |= MSP_RX_Port_IE;

	SPI_Strobe(SRX, Get_RX_FIFO); // Set radio to listen

	LPM3;	// Wait for the specified time

	SPI_Read_Status(RXBYTES, &status);	// Get packet status

	if(status & OverFlow) // If the buffer is overflowed dump it and don't read the values
	{
		return_status = RX_Buffer_Overflow;
		SPI_Strobe(SFRX, Get_RX_FIFO);
		goto Cleanup;
	}

	// Read the FIFO buffer into the out variable. IF there is no overflow then contents of the RXBYTES register is just the
	// number of bytes in the RX FIFO.
	SPI_Read_Burst(RXFIFO, out, status);

	return_status = Message_Recieved;


	Cleanup:
	// Restore old machine state
	MSP_RX_Port_IFG &= ~MSP_RX_Pin;			// Clear interrupt flags before exit
	SPI_Send(GDO_RX, Old_GDO);				// Set the GDO pin back to its old function
	MSP_RX_Port_IES = Old_MSP_RX_Port_IES;	// Restore the old interrupt edge select value
	MSP_RX_Port_IE = Old_MSP_RX_Port_IE;	// Restore the old interrupt enable value

	return return_status;
}
