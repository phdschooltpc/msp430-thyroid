#include <msp430.h>
#include <fann.h>

#include "thyroid_test.h"

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
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

    /* Assign testing data. */
    // data = fann_create_tests_from_header();

    /* Reset Mean Square Error. */
    fann_reset_MSE(ann);

    /* Run tests. */
    // for (i = 0; i < fann_length_train_data(data); i++) {
    //     calc_out = fann_test(ann, data->input[i], data->output[i]);
    //     printf("TT: %f should be %f, difference = %f\n",
    //             calc_out[0], data->output[i][0],
    //             (float) fann_abs(calc_out[0] - data->output[i][0]));
    // }
    for (i = 0; i < num_data; i++) {
        calc_out = fann_test(ann, input[i], output[i]);
        // printf("TT: %f should be %f, difference = %f\n",
        //         calc_out[2], output[i][2],
        //         (float) fann_abs(calc_out[2] - output[i][2]));
    }

    /* Print error. */
    // printf("MSE error on %d test data: %f\n", fann_length_train_data(data), fann_get_MSE(ann));
    printf("MSE error on %d test data: %f\n", num_data, fann_get_MSE(ann));

    /* Clean-up. */
    // fann_destroy_train(data); // TODO: take care when removing mallocs
    fann_destroy(ann);        // TODO: take care when removing mallocs

    __no_operation();

    // TODO: blink a LED

	return 0;
}
