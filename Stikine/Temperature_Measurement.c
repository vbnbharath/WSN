/*
 * Temperature_Measurement.c
 *
 *  Created on: Nov 25, 2015
 *      Author: cgoss
 */

#include <stdint.h>
#include <msp430.h>

uint8_t Get_Temperature()
{
	uint16_t ADCValues[8];
	uint16_t AvgValue = 0;

	// Configure the ADC10
	ADC10CTL0 = ADC10SHT_3  // Record a sample over 64 ADC clocks
			  + REFBURST 	// Only enable the reference during sample and hold
			  + REF2_5V 	// Use 2.5 V internal voltage reference
			  + ADC10ON 	// Turn on the ADC10
			  + ADC10IE;	// Enable interrupts


	// Configure the ADC10 channels
	ADC10CTL1 = INCH_10		// Channel 10 is temperature sensor
			  + CONSEQ_2	// Measure the same channel repeatedly
			  + ADC10DIV_3;	// Divide the clock by 3, to get ~30 us per sample

	// Configure the automatic data transfer
	ADC10DTC1 = 8; // Do 8 transfers for the block
	ADC10SA = (uint16_t)ADCValues; // Use the first pos'n in array as the first value


	ADC10CTL0 |= ADC10SC +  ENC;	// Fire ze missiles!

	LPM0;	// Sleep while the magic happens

	ADC10CTL0 = 0; // Turn the ADC10 off again

	int i = 0;
	for(i = 0; i < 8; i++)
	{
		AvgValue += ADCValues[i];
	}

	return AvgValue;

}
