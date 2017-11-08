#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#include "fann.h"
#include "thyroid_test.h"
#include "profiler.h"
/*Intermittent Tester*/
#include <tester.h>
#include <noise.h>

/* Debug variable. */
fann_type *calc_out;
static char string[] = "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! \n";

/**
 * main.c
 */
int main(void)
{
    /* Stop watchdog timer. */
    WDTCTL = WDTPW | WDTHOLD;

    /* Prepare LED. */
    PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode
                          // to activate previously configured port settings
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;

    /* Set master clock frequency to 8 MHz. */
//    CSCTL0 = CSKEY;
//    CSCTL1 &= ~DCOFSEL;
//    CSCTL1 |= DCOFSEL_6;
//    CSCTL3 &= ~(DIVS | DIVM);
//    CSCTL4 &= ~SMCLKOFF;

    /*Power load simulation*/
    /* You need to use these statements in the beginning your intermittent program*/
    //tester_autoreset(0, noise_3, 0);
    tester_notify_start();

    /* Fann structure. */
    struct fann *ann;

    uint32_t clk_cycles = 0;
    uint16_t i;

#ifdef PROFILE
    /* Start counting clock cycles. */
    profiler_start();
#endif // PROFILE

    /* Create network and read training data. */
    ann = fann_create_from_header();
    if (!ann) {
        return -1;
    }

#ifdef PROFILE
    /* Stop counting clock cycles. */
    clk_cycles = profiler_stop();

    /* Print profiling. */
    printf("ANN initialisation:\n"
           "-> execution cycles = %lu\n"
           "-> execution time = %.3f ms\n\n",
           clk_cycles, (float) clk_cycles / 8000);
#endif // PROFILE

    /* Reset Mean Square Error. */
    fann_reset_MSE(ann);

#ifdef PROFILE
    /* Start counting clock cycles. */
    profiler_start();
#endif // PROFILE

    /* Run tests. */
    for (i = 0; i < num_data; i++) {
        calc_out = fann_test(ann, input[i], output[i]);
#ifdef DEBUG
        /* Print results and errors (very expensive operations). */
        printf("Test %u:\n"
               "  result = (%f, %f, %f)\n"
               "expected = (%f, %f, %f)\n"
               "   delta = (%f, %f, %f)\n\n",
               i + 1,
               calc_out[0], calc_out[1], calc_out[2],
               output[i][0], output[i][1], output[i][2],
               (float) fann_abs(calc_out[0] - output[i][0]),
               (float) fann_abs(calc_out[1] - output[i][1]),
               (float) fann_abs(calc_out[2] - output[i][2]));
#else
        /* Breakpoint here and check the difference between calc_out[k] and
         * output[i][k], with k = 0, 1, 2. */
        __no_operation();
#endif // DEBUG
    }

#ifdef PROFILE
    /* Stop counting clock cycles. */
    clk_cycles = profiler_stop();

    /* Print profiling. */
    printf("Run %u tests:\n"
           "-> execution cycles = %lu (%lu per test)\n"
           "-> execution time = %.3f ms (%.3f ms per test)\n\n",
           i,
           clk_cycles, clk_cycles / i,
           (float) clk_cycles / 8000, (float) clk_cycles / 8000 / i);
#endif // PROFILE

    /* Print error. */
    printf("MSE error on %d test data: %f\n\n", num_data, fann_get_MSE(ann));

    /* Clean-up. */
    fann_destroy(ann);

    __no_operation();

    /*Report results*/
    /* You need to include that statement at the termination of your intermittent program*/
    //tester_send_data(0, string, 57);

    /* Turn on LED: Use for debugging */

    P1OUT |= BIT0;

    return 0;
}
