/*
 * tester.h
 *
 * Testing facilities, mainly for final evaluation.
 *
 * Created on: Nov 4, 2017
 *     Author: Carlo Delle Donne
 *
 * Credits: the autoreset functionality was taken from A. Majid's work
 *          available at https://github.com/amjadmajid/intermittent_power_supply_dev_tools
 */

#ifndef TESTER_H_
#define TESTER_H_

#include <msp430.h>
#include <stdint.h>

#ifdef FIXEDFANN
typedef long fann_type;
#else
typedef float fann_type;
#endif

#define NOISE_LEN 200

/**
 * Send result over UART.
 * TX pin: P2.5, RX pin: P2.6
 *
 * @param text_idx test index (from 0 to num_test - 1)
 * @param calc_out pointer to the result array
 * @param len length in byte of the data to send
 */
void tester_send_data(uint16_t test_idx, fann_type* calc_out, unsigned int len);

/**
 * Notify the starting by raising a GPIO.
 * Notification pin: P1.2
 */
void tester_notify_start(void);

/**
 * Notify the completion by raising a GPIO.
 * Notification pin: P1.2
 */
void tester_notify_end(void);

/**
 * Schedule an automatic reset defined by a time interval and a random noise.
 * WARNING: do not use while time profiling is enabled!
 *
 * @param interval base time interval in microseconds
 * @param noise_pattern pointer to the noise array
 * @param is_signed 1: noise_pattern is of type int16_t
 *                  0: noise_pattern is of type uint16_t
 */
void tester_autoreset(unsigned int interval, void* noise_pattern, uint8_t is_signed);

#endif /* TESTER_H_ */
