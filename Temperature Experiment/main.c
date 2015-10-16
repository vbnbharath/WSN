#include <msp430.h> // Base header files
#include <projectTypes.h> // pull in custom datatypes
#include <pinDefs.h>

/*
 * main.c
 */

// Stuff specific to the value line boards
#ifdef __MSP430G2553__
static inline void BoardInitValue(void);
static inline void TimerInitValue(void);
#endif

int main(void)
{

// Value line inits
#ifdef __MSP430G2553__
	BoardInitValue();
	TimerInitValue();
#endif

    LED1Reg |= LED1;

    __bis_SR_register(LPM0_bits + GIE); // Enter LPM0 w/ interrupt

    return 0; // Never get here
}

/**************************************************************************
 * Static inlines
 */

#ifdef __MSP430G2553

static inline void BoardInitValue()
{
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	// Clock setup
	DCOCTL = CALDCO_1MHZ;
	BCSCTL1 = CALBC1_1MHZ;
	BCSCTL2 = SELM_0; // Source MCLK and SMCLK from DCO
	BCSCTL3 = LFXT1S_2; // Use VLO for slow clock

	// Default pinsets for low power consumption

	// Set the mode of all ports to IO
	P1SEL = 0;
	P2SEL2 = 0;
	P2SEL = 0;
	P2SEL2 = 0;
	P3SEL = 0;
	P3SEL2 = 0;

	// Set the default output to low
	P1OUT = 0;
	P2OUT = 0;
	P3OUT = 0;

	// Set the direction to out
	P1DIR = 0xFF;
	P2DIR = 0xFF;
	P3DIR = 0XFF;

	// Configure USCI for UART comms
	UCA0CTL1 = UCSWRST; // USCI hold

	UCA0CTL0 = 0; // No parity, 8 chars, one stop bit, UART, Asynch
	UCA0CTL1 |= UCSSEL_2; // Use SMCLK for USCI module
	UCA0BR0 = 6; // Settings for 57600 Oversample Baud Rate
	UCA0BR1 = 0;
	UCA0MCTL = UCBRF_8 + UCOS16; // Modulation and oversample
	UCA0STAT = 0; // Clear status register

	// Pinsets for UART
	P1SEL |= BIT1 + BIT2;
	P1SEL2 |= BIT1 + BIT2;

	UCA0CTL1 &= ~UCSWRST; // USCI Start

	__delay_cycles(1000);

	UCA0TXBUF = 66;
}

static inline void TimerInitValue()
{
	// Timer A0 Slow Clock
	TACTL = TASSEL_1 + MC_0; // Set timer A source to SMCLK and stop the clock
	TAR = 0; // Initial count value is 0
	TACCR0 = 11999; // Count up to 12,000
	TACCTL0 = CCIE; // Enable interrupt on TACCR0
	TACCTL0 &= ~CCIFG;
	TACTL |= MC_1; // Start counting up to TACCR0

	// Timer A1 Fast Clock
	TA1CTL = TASSEL_2 + MC_0;
	TA1R = 0;
	TA1CCR0 = 49999; // Count up to 50000
	TA1CCTL0 = CCIE; // Enable interrupt on timer TA1CCR0
	TA1CCTL0 &= ~CCIFG;
	TA1CTL |= MC_1; // Start counting up to TACCR0

}
#endif

/**************************************************************************
 * Interrupt service routuines
 */

// Slow timer ISR
void __attribute__((__interrupt__(TIMER0_A0_VECTOR))) Timer1_A0_ISR(void)
{
	TACCTL0 &= ~CCIFG; // Clear the interrupt flag
	LED1Reg ^= LED1; // Toggle the LED
	UCA0TXBUF = 'S'; // Transmit an S
}

void __attribute__((__interrupt__(TIMER1_A0_VECTOR))) Timer1_A1_ISR(void)
{
	static int i = 0;

	TA1CCTL0 &= ~CCIFG; // Clear the interrupt flag

	if(i >= 20)
	{
		LED2Reg ^= LED2;
		i=0;
		UCA0TXBUF = 'F'; //Transmit an F
	}
	else
	{
		i++;
	}
}

