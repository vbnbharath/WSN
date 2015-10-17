/*
 * Spi_Library.h
 *
 *  Created on: Oct 16, 2015
 *      Author: cgoss
 */
#include <stdint.h>

#ifndef SPI_LIBRARY_H_
#define SPI_LIBRARY_H_

/*************************** Begin Value line pindefs ******************************/
#ifdef __MSP430G2553__
// SPI Pins
#define SOMI BIT6
#define SIMO BIT7
#define SPCLK BIT5
#define CS BIT7

// SPI Registers
#define Port_Reg_Sel P1SEL 				// Port function select 1
#define Port_Reg_Sel2 P1SEL2 			// Port function select 2
#define Port_Reg_Dir P1DIR				// Port direction select
#define Port_In P1IN					// Input values for SPI port
#define CS_Register P2OUT				// Chip select for the slave device
#define CS_RegisterDir P2DIR			// Direction register for chip select pin
#define USCI_Control_Reg0 UCB0CTL0 		// USCI control reg 0
#define USCI_Control_Reg1 UCB0CTL1		// USCI control reg 1
#define USCI_Modulation_Upper UCB0BR1	// Upper byte of modulation value
#define USCI_Modulation_Lower UCB0BR0	// Lower byte of modulation vale
#define USCI_Status_Reg UCB0STAT		// Status register
#define USCI_Interrupt_Flags UC0IFG		// Interrupt flag register
#define USCI_TX_Reg UCB0TXBUF			// Transmit buffer
#define USCI_RX_Reg UCB0RXBUF			// Receive buffer
#endif
/*************************** End Value line pindefs ******************************/



/*************************** Function prototypes ******************************/

void SPI_Init(void);
uint8_t SPI_Send(uint8_t address, uint8_t value);
uint8_t SPI_Read(uint8_t address, volatile uint8_t* out);
uint8_t SPI_Send_Burst(uint8_t address, uint8_t* value, uint8_t length);
uint8_t SPI_Read_Burst(uint8_t address, uint8_t* out, uint8_t length);


#endif /* SPI_LIBRARY_H_ */
