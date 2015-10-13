#include <msp430.h> // Base header files
#include <projectTypes.h> // pull in custom datatypes

/*
 * main.c
 */

static inline void BoardInit(void);
static inline void TimerInit(void);

int main(void)
{
	BoardInit();
	TimerInit();

    P1OUT |= BIT0 + BIT6; // Turn LED 1 out

    __bis_SR_register(LPM0_bits + GIE); // Enter LPM0 w/ interrupt

    return 0; // Never get here
}

/**************************************************************************
 * Static inlines
 */

static inline void BoardInit()
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
}

static inline void TimerInit()
{
	// Timer A0
	TACTL = TASSEL_1 + MC_0; // Set timer A source to SMCLK and stop the clock
	TAR = 0; // Initial count value is 0
	TACCR0 = 11999; // Count up to 12,000
	TACCTL0 = CCIE; // Enable interrupt on TACCR0
	TACTL |= MC_1; // Start counting up to TACCR0

	// Timer A1
	TA1CTL = TASSEL_2 + MC_0;
	TA1R = 0;
	TA1CCR0 = 49999; // Count up to 50000
	TA1CCTL0 = CCIE; // Enable interrupt on timer TA1CCR0
	TA1CTL |= MC_1; // Start counting up to TACCR0

}

/**************************************************************************
 * Interrupt service routuines
 */

// Slow timer ISR
void __attribute__((__interrupt__(TIMER0_A0_VECTOR))) Timer1_A0_ISR(void)
{
	TACCTL0 &= ~CCIFG; // Clear the interrupt flag
	P1OUT ^= BIT0; // Toggle the LEDs
}

void __attribute__((__interrupt__(TIMER1_A0_VECTOR))) Timer1_A1_ISR(void)
{
	static int i = 0;

	TA1CCTL0 &= ~CCIFG; // Clear the interrupt flag

	if(i >= 20)
	{
		P1OUT ^= BIT6;
		i=0;
	}
	else
	{
		i++;
	}
}

