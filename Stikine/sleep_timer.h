/*
 * sleep_timer.h
 *
 *  Created on: 11/17/15
 *      Author: Kirill
 */
/**
 * \file sleep_timer.h
 * \brief Sleep library for the MSP430
 *
 */

#ifndef SLEEP_TIMER_H_
#define SLEEP_TIMER_H_

#include <stdint.h>

/**
 * Puts the MCU to sleep for the specified number of ACLK rollovers and cycles.
 *
 * @param rollovers The number of rollovers to sleep for, each rollover is 65536 ticks of ACLK
 * @param cycles	The number of cycles to sleep for, in terms of ACLK ticks
 */
void Sleep_Timer (uint8_t rollovers, uint16_t cycles);

#endif /* SLEEP_TIMER_H_ */
