/*
 * Spi_Library.h
 *
 *  Created on: Oct 16, 2015
 *      Author: cgoss
 */
#include <stdint.h>
#include <CC110l.h>

#ifndef SPI_LIBRARY_H_
#define SPI_LIBRARY_H_

/*************************** Begin Value line pindefs ******************************/
#ifdef __MSP430G2553__
// SPI Pins on MSP430
#define SOMI BIT6
#define SIMO BIT7
#define SPCLK BIT5
#define CS BIT7

// GDO Pins on MSP430
#define MSP_RX_Pin BIT6					// Pin that the GDO flagging for RX is attached to

// GDO Register on CC110l, all 3 GDO pins have the same capabilities. By just mapping the registers here
// can define the GDO pins by behavior.
#define GDO_RX	IOCFG0					// Register controlling the GDO pin used to signal RX receive.

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

// GDO Ports on MSP430
#define MSP_RX_Port_DIR		P2DIR	// Direction register
#define MSP_RX_Port_OUT		P2OUT	// Out value register
#define MSP_RX_Port_REN		P2REN	// Resistor enable register
#define MSP_RX_Port_IN		P2IN	// In value register
#define MSP_RX_Port_IE		P2IE	// Interrupt enable register
#define MSP_RX_Port_IV		PORT2_VECTOR	// Port Interrupt vector
#define MSP_RX_Port_IFG		P2IFG	// Port interrupt flag
#endif
/*************************** End Value line pindefs ******************************/



/*************************** Function prototypes ******************************/

void SPI_Init(void);
uint8_t SPI_Send(uint8_t address, uint8_t value);
uint8_t SPI_Read(uint8_t address, uint8_t* out);
uint8_t SPI_Send_Burst(uint8_t address, uint8_t* value, uint8_t length);
uint8_t SPI_Read_Burst(uint8_t address, uint8_t* out, uint8_t length);
uint8_t SPI_Strobe(uint8_t strobe, uint8_t FIFO);


#endif /* SPI_LIBRARY_H_ */
