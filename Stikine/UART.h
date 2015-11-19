/*
 * UART.h
 *
 *  Created on: Nov 18, 2015
 *      Author: Brandon
 */

#ifndef UART_H_
#define UART_H_

void UARTSendArray(uint8_t *TxArray);
void Convert_to_Decimal(uint8_t Digits, uint8_t* Decimal);
void Convert_to_Hex(uint8_t* Dest, uint8_t* Payload, uint8_t Length);
#endif /* UART_H_ */
