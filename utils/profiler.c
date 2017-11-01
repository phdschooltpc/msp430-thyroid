/*
 * profiler.c
 *
 * Profiling facilities.
 *
 * Created on: Nov 1, 2017
 *     Author: Carlo Delle Donne
 */

#include "profiler.h"


uint32_t cycle_count;

/**
 * Start counting cycles.
 */
void profiler_start()
{
    cycle_count = 0;
    TA0CTL = TACLR;
    TA0CTL |= TASSEL__SMCLK | MC__CONTINUOUS | TAIE;
}


/**
 * Stop counting cycles and return cycle count.
 *
 * @return cycle count.
 */
uint32_t profiler_stop()
{
    cycle_count += TA0R;
    TA0CTL = 0;
    uint32_t number_of_isr = cycle_count / TIMER_PERIOD;
    cycle_count -= number_of_isr * (ISR_OVERHEAD + ISR_CYCLES);
    return cycle_count;
}


/**
 * Timer0_A1 interrupt service routine
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) TIMER0_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TA0IV, TAIV__TAIFG))
    {
        case TAIV__NONE:   break;           // No interrupt
        case TAIV__TACCR1: break;           // CCR1 not used
        case TAIV__TACCR2: break;           // CCR2 not used
        case TAIV__TACCR3: break;           // reserved
        case TAIV__TACCR4: break;           // reserved
        case TAIV__TACCR5: break;           // reserved
        case TAIV__TACCR6: break;           // reserved
        case TAIV__TAIFG:                   // overflow
            cycle_count += TIMER_PERIOD;
            break;
        default: break;
    }
}
