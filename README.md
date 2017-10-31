# Thyroid disease example for MSP430

Machine learning example running on MSP430, _not_ intermittent-safe.

### How to use

Just clone/download the repository and import the project in CCS.

### Program

The main function _dynamically_ allocates an Artificial Neural Network (ANN) using network parameters provided in `database/thyroid_trained.h`. This header file is constructed from `database/thyroid_trained.net`. If you are familiar with some machine learning concepts, you may try to tweak network parameters in the `.net` file, and re-generate the header file using the provided script as follows:

    cd database
    ./strip-train-data thyroid_trained.net

After the ANN has been allocated, a certain number of input tests are fed to the network, and the resulting inference is compared with the expected output. Input and output vectors are provided in `database/thyroid_test.h`. The number of performed tests can be increased (or decreased) re-generating the header file:

    cd database
    ./strip-test-data thyroid.test [number_of_tests]

The `.data` file contains 3600 tests, which is the maximum value for `number_of_tests`. Currently, 250 tests are uploaded, on the FRAM, and run. The more tests, the more accurate the Mean Square Error (MSE) for the network. Nevertheless, the FRAM is limited in size, so all the 3600 tests will not fit. During the evaluation of your work, a fixed amount of tests will be run.

### Suggestions

1. first suggestion
2. second suggestion