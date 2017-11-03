/*
 * profiler.h
 *
 * Accurate time profiling facility, using Timer (and its interrupt).
 *
 * NOTE: the time spent inside Timer interrupts is accurately subtracted
 *       from the final measurement.
 *
 * Created on: Nov 1, 2017
 *     Author: Carlo Delle Donne
 */

#ifndef PROFILER_H_
#define PROFILER_H_

#include <msp430.h>
#include <stdint.h>


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
