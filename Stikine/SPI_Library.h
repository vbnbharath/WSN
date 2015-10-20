/*
 * Spi_Library.h
 *
 *  Created on: Oct 16, 2015
 *      Author: cgoss
 */
#include <stdint.h>

#ifndef SPI_LIBRARY_H_
#define SPI_LIBRARY_H_


/*************************** Function prototypes ******************************/

void SPI_Init(void);
uint8_t SPI_Send(uint8_t address, uint8_t value);
uint8_t SPI_Read(uint8_t address, uint8_t* out);
uint8_t SPI_Send_Burst(uint8_t address, uint8_t* value, uint8_t length);
uint8_t SPI_Read_Burst(uint8_t address, uint8_t* out, uint8_t length);
uint8_t SPI_Strobe(uint8_t strobe, uint8_t FIFO);


#endif /* SPI_LIBRARY_H_ */
