/*
 * Spi_Library.h
 *
 *  Created on: Oct 16, 2015
 *      Author: cgoss
 */
#include <stdint.h>

#ifndef SPI_LIBRARY_H_
#define SPI_LIBRARY_H_

// Outward facing pins/registers for the radio
#ifdef __MSP430G2553__
// GDO Pins on MSP430
#define MSP_RX_Pin BIT3					// Pin that the GDO flagging for RX is attached to

// GDO Register on CC110l, all 3 GDO pins have the same capabilities. By just mapping the registers here
// can define the GDO pins by behavior.
#define GDO_RX	IOCFG0					// Register controlling the GDO pin used to signal RX receive.

#endif
/*************************** Function prototypes ******************************/

void SPI_Init(void);
uint8_t SPI_Send(uint8_t address, uint8_t value);
uint8_t SPI_Read(uint8_t address, uint8_t* out);
uint8_t SPI_Send_Burst(uint8_t address, uint8_t* value, uint8_t length);
uint8_t SPI_Read_Burst(uint8_t address, uint8_t* out, uint8_t length);
uint8_t SPI_Strobe(uint8_t strobe, uint8_t FIFO);
uint8_t SPI_Read_Status(uint8_t status_reg, uint8_t* out);


#endif /* SPI_LIBRARY_H_ */
