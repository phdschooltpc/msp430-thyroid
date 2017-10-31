#include <msp430.h>
#include <fann.h>

#include "thyroid_test.h"

// TODO: add profiling facilities
// TODO: remove unnecessary functions

/**
 * main.c
 */
int main(void)
{
    /* Stop watchdog timer. */
    WDTCTL = WDTPW | WDTHOLD;

    /* Prepare LED. */
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;
	
    /* Set master clock frequency to 8 MHz. */
    CSCTL0 = CSKEY;
    CSCTL1 &= ~DCOFSEL;
    CSCTL1 |= DCOFSEL_6;
    CSCTL3 &= ~(DIVS | DIVM);

	/* Fann structures. */
    struct fann *ann;
    struct fann_train_data *data;

    /* Test-related variables. */
    fann_type *calc_out;
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
        /* Print one of the three results ([0] or [1] or [2]). */
        printf("Test result: %f, expected: %f, difference = %f\n",
                calc_out[2], output[i][2],
                (float) fann_abs(calc_out[2] - output[i][2]));
    }
#endif // DEBUG

    /* Print error. */
    printf("MSE error on %d test data: %f\n", num_data, fann_get_MSE(ann));

    /* Clean-up. */
    fann_destroy(ann);

    __no_operation();

    /* Turn on LED. */
    P1OUT |= BIT0;

    return 0;
}
