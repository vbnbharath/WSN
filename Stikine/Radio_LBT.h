/*
 * Radio_LBT.h
 *
 *  Created on: Oct 22, 2015
 *      Author: cgoss
 */

/**
 * \file Radio_LBT.h
 * \brief Library for transmitting using listen before talk with acknowledgement
 *
 *
 */

#include<stdint.h>

#ifndef RADIO_LBT_H_
#define RADIO_LBT_H_

/// \name Enumerated type for transmit statuses
//@{
typedef enum{
Transmit_Success, 	///< Transmission was successful
Already_Transmitting,    	///< Radio was already in the process of transmitting
Ack_Timeout,		///< Acknowledgement not Rec'd before timeout expired
TX_Buffer_Overflow,  ///< Message overflowed the TX FIFO
TX_Collision,	///< Channel was not clear after transmit
Channel_Busy	///<
} LBT_Status;
//@}

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
LBT_Status LBT_Send(uint16_t address, uint8_t* message, uint8_t length);

#endif /* RADIO_LBT_H_ */
