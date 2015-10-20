/**
 *\file SPI_Library.c
 *\brief Setup and control library for the SPI link to the CC110l
 *
 * Ports and pins for this library are defined in SPI_Library.h, there are ifdef blocks to target different controllers.
 */

#include <stdint.h>
#include <msp430.h>
#include <CC110l.h>
#include <SPI_Pins.h>

// Function to check that address passed into SPI functions is valid
static uint8_t Address_Bad(uint8_t address)
{
	if(address & (BIT7 + BIT6)) // If BIT7 hi throw error
	{
		return 1; // Return 1 if bad
	}
	else
	{
		return 0; // Return 0 if good
	}
}

// Function to spinwait if the radio is not in "ready" state
static void Wait_For_CCWake()
{
	// Spinlock until radio wakes
	while(Port_In & SOMI); // If SOMI is HI, chip is sleeping
}

/**
 * \brief Initialization function for the SPI link
 *
 */
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
	// Setup the RX flag pin
	MSP_RX_Port_DIR &= ~MSP_RX_Pin; // Set the RX pin for input
	MSP_RX_Port_OUT &= ~MSP_RX_Pin; // Set the RX pin to pull down
	MSP_RX_Port_REN |= MSP_RX_Pin;	// Enable pullup resistor

}

/**
 * \brief A function for sending a single byte to the radio
 *
 *
 * @param address The register in the CC110l that the byte is to be written to.
 * @param value The value to write to the target register
 * @return The status byte from the CC110l
 */
uint8_t SPI_Send(uint8_t address, uint8_t value)
{
	uint8_t status; // The return value

	USCI_Interrupt_Flags &= ~00001111; // Clear the flags

	if(Address_Bad(address))
	{
		return BIT7;
	}

	CS_Register &= ~CS; // Pull CS low

	Wait_For_CCWake(); // Wait for SOMI to go LO

	// TX buffer empties into shift register in one cycle, so we can write twice without delay
	USCI_TX_Reg = address; // Send address
	USCI_TX_Reg = value; // Send the actual value,
	// Spinlock until TX finish to ensure CS is held until the transmission is complete
	while(!(USCI_Interrupt_Flags & UCB0RXBUF));

	status = USCI_RX_Reg; // Read the status byte from the input buffer
	CS_Register |= CS; // Pull CS HI

	return status;
}

uint8_t SPI_Read(uint8_t address, uint8_t *out)
{
	uint8_t status;

	*out = USCI_RX_Reg; // Clear the flags

	// Check for valid address
	if(Address_Bad(address))
	{
		return BIT7;
	}

	CS_Register &= ~CS; // Pull CS low

	Wait_For_CCWake(); // Wait for SOMI to go LO

	USCI_TX_Reg = address | READ_SINGLE; // Send address with MSB HI, indicates read
	while(!(USCI_Interrupt_Flags & UCB0RXIFG));
	status = USCI_RX_Reg; // Save the status byte
	USCI_TX_Reg = 0xFF; // Gotta send it something to make the SPI hardware run, this will be ignored by the CC110l

	// Spinlock until RX finish
	while(!(USCI_Interrupt_Flags & UCB0RXIFG));

	*out = USCI_RX_Reg; // Grab the value

	CS_Register |= CS; // Pull CS HI

	return status;
}

uint8_t SPI_Send_Burst(uint8_t address, uint8_t* value, uint8_t length)
{
	int i;
	uint8_t status;

	if(Address_Bad(address))
	{
		return BIT7;
	}

	CS_Register &= ~CS; // Pull CS low
	Wait_For_CCWake(); // Wait for SOMI to go LO

	USCI_TX_Reg = address | WRITE_BURST; // Send address with burst bit set
	USCI_TX_Reg = value[0]; // Send address

	for(i=1; i< length; i++)
	{
		while(!(USCI_Interrupt_Flags & UCB0TXIFG)); // Wait for ready
		USCI_TX_Reg = value[i];
	}

	while(!(USCI_Interrupt_Flags & UCB0RXIFG));
	CS_Register |= CS; // Pull CS HI
	status = USCI_RX_Reg; // Read the status byte from the input buffer
	return status;
}

uint8_t SPI_Read_Burst(uint8_t address, uint8_t* out, uint8_t length)
{
	uint8_t i;
	uint8_t status;

	i = USCI_RX_Reg; // Clear the flags

	if(Address_Bad(address))
	{
		return BIT7;
	}

	CS_Register &= ~CS; // Pull CS low
	Wait_For_CCWake(); // Wait for SOMI to go LO

	USCI_TX_Reg = address | READ_BURST; // Send address with burst bit set and read bit set
	while(!(USCI_Interrupt_Flags & UCB0RXIFG)); // Wait for ready
	status = USCI_RX_Reg;

	for(i = 0; i < length; i++)
	{
		USCI_TX_Reg = 0xFF;
		while(!(USCI_Interrupt_Flags & UCB0RXIFG)); // Wait for ready
		out[i] = USCI_RX_Reg;
	}

	CS_Register |= CS; // Pull CS HI
	return status;
}

uint8_t SPI_Strobe(uint8_t strobe, uint8_t FIFO)
{
	uint8_t status;

	status = USCI_RX_Reg; // Clear UCB RX flag

	CS_Register &= ~CS; // Pull CS low
	Wait_For_CCWake(); // Wait for SOMI to go LO

	USCI_TX_Reg = strobe | FIFO; // Pick either the TX or RX FIFO for the
	while(!(USCI_Interrupt_Flags & UCB0RXIFG)); // Wait for ready
	status = USCI_RX_Reg;

	CS_Register |= CS; // Pull CS HI
	return status;
}
