#ifndef PROFILER_H_
#define PROFILER_H_

#include <msp430.h>
#include <stdint.h>

#define TIMER_PERIOD 0x00010000

#define ISR_OVERHEAD 11 // Cycles needed to switch to interrupt and back
                        // Source: http://www.ti.com/lit/ug/slau367n/slau367n.pdf (pp. 52-53)

#define ISR_CYCLES   17 // Cycles spent inside IRS (measured)


/**
 * Start counting cycles.
 */
void profiler_start();


/**
 * Stop counting cycles and return cycle count.
 *
 * @return cycle count.
 */
uint32_t profiler_stop();

#endif /* PROFILER_H_ */
