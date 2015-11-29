/*
 * Temperature_Measurement.c
 *
 *  Created on: Nov 25, 2015
 *      Author: cgoss
 */

#include <stdint.h>
#include <msp430.h>

#define Sample_Count 8		///> Number of samples to average for a temperature measurement

uint8_t Get_Temperature()
{
	volatile uint16_t ADCValues[Sample_Count];
	volatile int16_t AvgValue = 0;
	volatile int32_t Temperature = 0;
	volatile int16_t Cal30;
	volatile uint16_t Cal85;

	ADC10CTL0 = ADC10ON; 	// Turn on the ADC10 first

	// Configure the ADC10
	ADC10CTL0 = ADC10SHT_3  // Record a sample over 64 ADC clocks
			  + REFBURST 	// Only enable the reference during sample and hold
			  + SREF_1		// Use internal reference
			  + ADC10ON 	// Turn on the ADC10
			  + REFON		// Turn on reference generator
			  + ADC10IE		// Enable interrupts
			  + MSC;		// Multiple sample conversion

	ADC10CTL1 = INCH_10 	// Channel 10 is temperature sensor
			  + CONSEQ_2	// Measure the same channel repeatedly
			  + ADC10DIV_3;	// Divide the clock by 3, to get ~30 us per sample


	// Configure the automatic data transfer
	ADC10DTC0 = 0; // Single block mode
	ADC10DTC1 = 8; // Do 8 transfers for the block
	ADC10SA = (uint16_t)ADCValues; // Use the first pos'n in array as the first value
	ADC10CTL0 |= ENC;	// Enable conversion
	ADC10CTL0 |= ADC10SC; // Start

	LPM0;	// Sleep while the magic happens

	ADC10CTL0 = 0; // Turn the ADC10 off again

	// Sum the values
	int i = 0;
	for(i = 0; i < Sample_Count; i++)
	{
		AvgValue += ADCValues[i];
	}

	AvgValue = AvgValue / Sample_Count;		// Take the average

	// Get values from the TLV
	Cal30 = *((uint16_t*)TLV_ADC10_1_TAG_ + CAL_ADC_15T30 + 1);
	Cal85 = *((uint16_t*)TLV_ADC10_1_TAG_ + CAL_ADC_15T85 + 1);

	// Compute the temperature using integer math
	Temperature = (AvgValue - Cal30) * ( 5500000 / (Cal85 - Cal30) ) + 3000050;

	// Shift back down
	Temperature = Temperature/100;

	return Temperature;
}
