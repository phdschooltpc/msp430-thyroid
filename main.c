#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#include "fann.h"

#include "thyroid_test.h"

// TODO: add profiling facilities
// TODO: remove unnecessary functions
// TODO: try --data_model=full to fit more tests

/* Debug variable. */
fann_type *calc_out;

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
    CSCTL0 = CSKEY;
    CSCTL1 &= ~DCOFSEL;
    CSCTL1 |= DCOFSEL_6;
    CSCTL3 &= ~(DIVS | DIVM);

	/* Fann structures. */
    struct fann *ann;
    unsigned int i;

    /* Create network and read training data. */
    ann = fann_create_from_header();
    if (!ann) {
        return -1;
    }

    /* Reset Mean Square Error. */
    fann_reset_MSE(ann);

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

    /* Print error. */
    printf("MSE error on %d test data: %f\n", num_data, fann_get_MSE(ann));

    /* Clean-up. */
    fann_destroy(ann);

    __no_operation();

    /* Turn on LED. */
    P1OUT |= BIT0;

    return 0;
}
