/*
 * Radio_LBT.h
 *
 *  Created on: Oct 22, 2015
 *      Author: cgoss
 */

/**
 * \file Radio.h
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

/// \name Structure to represent a recieved message from the radio
//@{
struct Listen_Struct {
	LBT_Status Status;		///< An LBT_Status, indicates whether timeout occured or a message was recieved
	uint8_t address;		///< The address of the sender
	uint8_t signal;			///< The RSSI from the transmission
	uint8_t length;			///< The length of the message payload
	uint8_t payload[60];	///< The actual message which was sent
};
//@}

/**
 * \brief Sends a message over the CC110l radio using listen before talk.
 *
 * Function checks the status of the radio prior to tansmission. If the state is anything but 'idle' the
 * function abort and return a status indicating the fail state. Maximum message length is 59 bytes because
 * of the extra bytes included for the hardware packet handling and appended RSSI and CRC bytes at the receiver.
 *
 * Blocking function, returns after the transmission is complete.
 *
 * @param dest_address The address of the recipient. This address is what the CC110l hardware address filter operates against
 * @param source_address The address of this node, not used for hardware filtering
 * @param message The payload of the transmission, can be up to 59 bytes long
 * @param length The length of the payload. The function will adjust this value to accomodate the addresses being attached to the
 * transmission
 * @return Returns an LBT_Status enum. This can be checked to ensure the transmission was successful.
 */
LBT_Status LBT_Send(uint8_t dest_address, uint8_t source_address, uint8_t *message, uint8_t length);

/**
 * \brief Puts the radio into listen mode for the specified amount of time. Blocking function, does not return until timeout
 * occurs or a message is recieved.
 *
 * @param timeoutPeriod The amount of time, in ACLK cycles to wait for a transmission to come in.
 * @return Returns a Listen_Struct which contains a status field and the payload.
 */
struct Listen_Struct LBT_Listen(uint16_t timeoutPeriod);

/**
 * \brief Sends a message without checking for clear channel first
 * @param dest_address Address of the message recipient
 * @param source_address Address of the sender
 * @param message The message payload
 * @param length Length of the payload
 * @return
 */
LBT_Status TDMA_Send(uint8_t dest_address, uint8_t source_address, uint8_t *message, uint8_t length);

/**
* \brief Gets a 16 bit unsigned integer and divides it into two 8 bit unsigned integers
* @param two_byte_data Input 16 bit integer
* @param *ptr Points where to store the result
*/
void int_divide (uint16_t two_byte_data, uint8_t *ptr);

/**
* \brief Gets two 8 bit unsigned integers and merges them into 16 bit unsigned integer
* @param lower Lower 8 bits to be merged
* @param upper Upper 8 bits to be merged
*/
uint16_t int_merge (uint8_t lower, uint8_t upper);

#endif /* RADIO_LBT_H_ */
