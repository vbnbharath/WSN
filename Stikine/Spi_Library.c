/*
 * Spi_Library.c
 *
 *  Created on: Oct 16, 2015
 *      Author: cgoss
 */

#include <stdint.h>
#include <msp430.h>
#include <Spi_Library.h>

static uint8_t Address_Bad(uint8_t address)
{
	if(address & BIT7) // If BIT7 hi throw error
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static void Wait_For_CCWake()
{
	// Spinlock until radio wakes TODO: Make this interrupt based and LPM0 instead
	while(Port_In & SOMI); // If SOMI is HI, chip is sleeping
}


// Init function
void SPI_Init(void)
{
	// Configure controller IO for SPI pins
	// Both Sel and Sel2 values for the respective pins should be set HI
	Port_Reg_Sel |= SOMI + SIMO + SPCLK;
	Port_Reg_Sel2 |= SOMI + SIMO + SPCLK;

	// Configure the GPIO for chip select
	CS_RegisterDir |= CS;

	// CC110l uses active low for chip select, so drive CS HI
	CS_Register |= CS;

	// Set software reset bit to hold USCI module
	USCI_Control_Reg1 = UCSWRST;

	// Read first edge, clock active HI, MSB bit order, master mode, synchronous (SPI)
	USCI_Control_Reg0 = UCCKPH + UCMSB + UCMST + UCSYNC;

	// Use SMCLK for SPI
	USCI_Control_Reg1 |= UCSSEL_2;

	// Don't modulate, use full clock speed
	USCI_Modulation_Upper = 0;
	USCI_Modulation_Lower = 0;

	// Clear the status register and interrupt flags before use
	USCI_Status_Reg = 0;
	USCI_Interrupt_Flags = 0;

	// Ready to go, release software reset
	USCI_Control_Reg1 &= ~UCSWRST;
}

// Sends one value to the radio, returns the radio status.
// If the address is bogus a value of 0 is returned
// 0 is never returned by the radio
uint8_t SPI_Send(uint8_t address, uint8_t value)
{
	uint8_t status; // The return value

	if(Address_Bad(address))
	{
		return 0;
	}

	CS_Register &= ~CS; // Pull CS low

	Wait_For_CCWake(); // Wait for SOMI to go LO

	USCI_TX_Reg = address; // Send address

	// Spinlock until TX finish TODO: Also make this interrupt based and LPM0 instead
	while(!(USCI_Interrupt_Flags & UCB0RXIFG));

	USCI_TX_Reg = value; // Send the actual value

	// Spinlock until TX finish, makes function blocking, can call it repeatedly without worries of buffer overwrites.
	while(!(USCI_Interrupt_Flags & UCB0RXIFG));

	CS_Register |= CS; // Pull CS HI

	status = USCI_RX_Reg; // Read the status byte from the input buffer

	return status;
}

uint8_t SPI_Read(uint8_t address)
{
	uint8_t register_value;

	// Check for valid address TODO: Consider making the register value an OUT parameter and return a status value instead?
	if(Address_Bad(address))
	{
		return 0;
	}

	CS_Register &= ~CS; // Pull CS low

	Wait_For_CCWake(); // Wait for SOMI to go LO

	USCI_TX_Reg = address + BIT7; // Send address with MSB HI, indicates read

	// Spinlock until TX finish TODO: Also make this interrupt based and LPM0 instead
	while(!(USCI_Interrupt_Flags & UCB0RXIFG));

	USCI_TX_Reg = 0xFF; // Gotta send it something to make the SPI hardware run, this will be ignored by the CC110l

	// Spinlock until RX finish TODO: Also make this interrupt based and LPM0 instead
	while(!(USCI_Interrupt_Flags & UCB0RXIFG));

	register_value = USCI_RX_Reg; // Grab the value

	CS_Register |= CS; // Pull CS HI

	return register_value;
}
