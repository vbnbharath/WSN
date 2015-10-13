#include <msp430.h> // Base header files

/*
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer



    P1DIR |= BIT0; // Configure LED 1 for output
    P1OUT |= BIT0; // Turn the LED on

    P1DIR |= BIT6;
    P1OUT &= ~BIT6;

    TACTL = TASSEL_1 + MC_0; // Set timer A source to SMCLK and stop the clock
    TAR = 0; // Initial count value is 0

    TACCR0 = 10000; // Count up to 10,001
    TACCTL0 = CCIE; // Enable interrupt on TACCR0

    TACTL |= MC_1; // Start counting up to TACCR0

    __bis_SR_register(GIE); // Enter LPM0 w/ interrupt

    while(1)
    {
    	__delay_cycles(1000);
    }

    __bis_SR_register(LPM0_bits + GIE); // Enter LPM0 w/ interrupt

    return 0; // Never get here
}

void __attribute__((__interrupt__(TIMER0_A0_VECTOR))) Timer1_A0_ISR(void)
{
	TACCTL0 &= ~CCIFG; // Clear the interrupt flag
	P1OUT ^= BIT0 + BIT6; // Toggle the LEDs
}
