#include <msp430.h>
#include <tester.h>
#include <stdbool.h>

#pragma PERSISTENT(noise_idx)
unsigned int noise_idx = 0;

#pragma PERSISTENT(next_test_idx)
unsigned int next_test_idx = 0;

uint16_t csctl1, csctl3, csctl4;


void uart_init()
{
	PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode
                                // to activate previously configured port settings

    P2SEL1 |= BIT5 | BIT6;      // P2.5 for TX, P2.6 for RX
    P2SEL0 &= ~(BIT5 | BIT6);   // set for input Secondary Module Function
                                // SEL0 = 0, SEL1 = 1

    UCA1CTLW0 = UCSWRST;        // set UCSWRST to allow UART configuration
    UCA1CTLW0 |= UCSSEL__SMCLK; // choose SMCLK as f_baudclk

    /*
     * Set:
     * oversampling mode -> UCOS16 = 1
     * given N = f_baudclk / BAUD_RATE = 8000000 / 19200 = 416.66
     * UCBRx = INT(N/16) = 26 (see page 585)
     * UCBRFx = INT([N/16] - INT(N/16)] * 16) = 0 (see page 585)
     * UCBRSx (lookup table) = 0xB6 (see page 585)
     */
    UCA1MCTLW |= UCOS16;
    UCA1BRW = 26;
    UCA1MCTLW |= 0xB600;

    // UCA0BR0 = 8;
    // UCA0MCTLW |= 0xD600;
    // UCA0BR1 = 0;

    UCA1CTLW0 &= ~UCSWRST;      // clear UCSWRST to enable UART operation
}


void set_clk_to_8_MHz()
{
    /* Save clock registers' state */
    csctl1 = CSCTL1 & DCOFSEL;
    csctl3 = CSCTL3 & (DIVS | DIVM);
    csctl4 = CSCTL4 & SMCLKOFF;

    /* Set clock frequency to 8 MHz (SMCLK and MCLK). */
    CSCTL0 = CSKEY;
    CSCTL1 &= ~DCOFSEL;
    CSCTL1 |= DCOFSEL_6;
    CSCTL3 &= ~(DIVS | DIVM);
    CSCTL4 &= ~SMCLKOFF;
}


void set_smclk_to_1_MHz()
{
    CSCTL0 = CSKEY;
    CSCTL1 &= ~DCOFSEL;
    CSCTL1 |= DCOFSEL_6;
    CSCTL3 &= ~DIVS;
    CSCTL3 |= DIVS_3;
    CSCTL4 &= ~SMCLKOFF;
}


void restore_clock()
{
    CSCTL0 = CSKEY;
    CSCTL1 &= ~DCOFSEL;
    CSCTL1 |= csctl1;
    CSCTL3 |= csctl3;
    CSCTL4 |= csctl4;
}


void uart_send_data(uint16_t test_idx, fann_type* calc_out, unsigned int len)
{
	static bool uart_initialized = false;
	uint8_t* data;

	if (!uart_initialized) {
		uart_init();
		uart_initialized = true;
	}

    set_clk_to_8_MHz();

	/* Send test index. */
	data = (uint8_t*) &test_idx;
	while (!(UCA1IFG & UCTXIFG));
	UCA1TXBUF = *data;
	data++;
	while (!(UCA1IFG & UCTXIFG));
	UCA1TXBUF = *data;

	/* Send output data. */
	data = (uint8_t*) calc_out;
	while (len--) {
		while (!(UCA1IFG & UCTXIFG));
		UCA1TXBUF = *data;
		data++;
	}

    while (!UCTXCPTIFG);
    __delay_cycles(80000); // delay 10 ms

    restore_clock();
}


void tester_send_data(uint16_t test_idx, fann_type* calc_out, unsigned int len)
{
	uint16_t timer_status = TA0CTL & MC;
    TA0CTL &= ~MC;                           // halt the timer
    if (test_idx < next_test_idx) {
        TA0CTL |= timer_status;              // restore timer status
        return;
    }
    uart_send_data(test_idx, calc_out, len); // send data
    next_test_idx++;
    TA0CTL |= timer_status;                  // restore timer status
}


void tester_notify_start(void)
{
	uint16_t timer_status = TA0CTL & MC;
	TA0CTL &= ~MC;          // halt the timer

    PM5CTL0 &= ~LOCKLPM5;   // Disable the GPIO power-on default high-impedance mode
                            // to activate previously configured port settings

    P1DIR |= BIT2;          // Set P1.2 to output direction
    P1OUT |= BIT2;          // Set P1.2 to HIGH
    __delay_cycles(800000); // Wait 100 ms
    P1OUT &= ~BIT2;         // Set P1.2 to LOW

    TA0CTL |= timer_status; // restore timer status
}


void tester_notify_end(void)
{
	tester_notify_start();
}


void tester_autoreset(unsigned int interval, void* noise_pattern, uint8_t is_signed)
{
    int32_t ccr;

    if (is_signed) {
        ccr = interval + ((int16_t*) noise_pattern)[noise_idx];
    }
    else {
        ccr = interval + ((uint16_t*) noise_pattern)[noise_idx];
    }

    if (ccr < 0) ccr = 0;
    if (ccr > 0xFFFF) ccr = 0xFFFF;

    set_smclk_to_1_MHz();

    TA0CCTL0 = CCIE;
    TA0CCR0 = (uint16_t) ccr;
    TA0CTL = TASSEL__SMCLK | MC__UP;

    if (++noise_idx >= NOISE_LEN) {
        noise_idx = 0;
    }
    
    __bis_SR_register(GIE);       // enable general interrupt
}


// Timer0_A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer0_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	// TODO: modify as needed
    __bic_SR_register(GIE);       // disable general interrupt
    __no_operation();
    PMMCTL0 = PMMPW | PMMSWBOR;
}
