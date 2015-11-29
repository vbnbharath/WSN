/*
 * Temperature_Measurement.h
 *
 *  Created on: Nov 25, 2015
 *      Author: cgoss
 */

/**
 * \file Temperature_Measurement.h
 * \brief ADC Temperature measurement
 */

#include <stdint.h>

#ifndef TEMPERATURE_MEASUREMENT_H_
#define TEMPERATURE_MEASUREMENT_H_

/**
 * \brief A function to take a temperature measurement using the on-board temperature sensor
 *
 * Computes the silicon temperature of the MSP430. The temperature is reported as a integer value of
 * thousands of degrees celsius, IE 29551 is 29.551 celsius.
 *
 * Takes roughly 2 ms to complete
 * @return	The integer value, in hundreths of degrees celsius
 */
uint16_t Get_Temperature();

#endif /* TEMPERATURE_MEASUREMENT_H_ */
