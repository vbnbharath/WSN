/*
 * Radio_LBT.h
 *
 *  Created on: Oct 22, 2015
 *      Author: cgoss
 */

/**
 * \file Radio_LBT.h
 * \brief Library for transmitting using listen before talk
 *
 *
 */

#include<stdint.h>

#ifndef RADIO_LBT_H_
#define RADIO_LBT_H_

/// \name Enumerated type for tx/rx statuses
//@{
typedef enum{
Transmit_Success, 	///< Transmission was successful
Radio_Busy,    	///< Radio was not in idle state
Ack_Timeout,		///< Acknowledgement not Rec'd before timeout expired
TX_Buffer_Overflow,  ///< Message overflowed the TX FIFO
Channel_Busy,	///< Channel clear assessment returns negative
RX_Timeout, 	///< Specified listen period reached
Message_Recieved, ///< Successfully got a message
RX_Buffer_Overflow ///< RX FIFO was overflowed
} LBT_Status;
//@}

struct Listen_Struct {
	LBT_Status Status;
	uint8_t address;
	uint8_t signal;
	uint8_t length;
	uint8_t payload[60];
};

/**
 * \brief Sends a message over the radio using listen before talk
 *
 *
 *
 * @param address The recipients address
 * @param message The message to send
 * @param length Length of the message
 * @return LBT_Status enumerable
 */
LBT_Status LBT_Send(uint8_t dest_address, uint8_t source_address, uint8_t *message, uint8_t length);
struct Listen_Struct LBT_Listen(uint16_t timeoutPeriod);

#endif /* RADIO_LBT_H_ */
