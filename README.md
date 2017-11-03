# Thyroid disease example for MSP430

Machine learning example running on MSP430, _not_ intermittent-safe. It currently uses _floating-points_.

## How to use

Just clone/download the repository and import the project in CCS, build and run.

## Program

The main function _dynamically_ allocates an Artificial Neural Network (ANN) using network parameters provided in `database/thyroid_trained.h`. This header file is constructed from `database/thyroid_trained.net`. If you are familiar with some machine learning concepts, you may try to tweak network parameters and re-train the network using [FANN](https://github.com/libfann/fann) (or just use the example in [`phdschooltpc/thyroid_example_FANN`](https://github.com/phdschooltpc/thyroid_example_FANN)). Grab the resulting `.net` file, put it inside the `database` folder and re-generate the header file using the provided script as follows:

```bash
cd database
./strip-train-data thyroid_trained.net
```

After the ANN has been allocated, a certain number of input tests are fed to the network, and the resulting inference is compared with the expected output. Input and output vectors are provided in `database/thyroid_test.h`. The number of performed tests can be increased (or decreased) re-generating the header file:

```bash
cd database
./strip-test-data thyroid.test [number_of_tests]
```

The `.test` file contains 3600 tests, which is the maximum value for `number_of_tests`. Currently, 250 tests are uploaded, on the FRAM, and run. The more tests, the more accurate the Mean Square Error (MSE) for the network. Nevertheless, the FRAM is limited in size, so all the 3600 tests will not fit. During the evaluation of your work, a fixed amount of tests will be run.

## Suggestions

Have a look at the code, then:

1. Try to spot things that can be optimised, since execution time is crucial for an intermittently-powered device.
2. Try to think of a possible approach for splitting the program in different tasks<sup>1</sup>, or any suitable solution for it to run consistently on a transiently-powered system.

Note: some optimisations may lead to loss in accuracy (e.g. fixed- instead of floating-points), take it into account.

<sup>1</sup>You can find a sample taskified program in [`phdschooltpc/msp430-intermittent-example`](https://github.com/phdschooltpc/msp430-intermittent-example). It serves as a tutorial to use the intermittent-safe library we provide you (the library is imported in this project as well, check the `utils` folder).

## Known issues

#### Compiler version
The project was built and tested with the latest MSP430 compiler version (17.9.0). If you don't have it, please download it from CCS by going to `Help > Install New Software` and typing "Code Generation Tools Updates" in the search bar. On Linux distributions you may have to run CCS as a superuser, e.g. in Ubuntu
```bash
sudo /opt/ti/ccsv7/eclipse/ccstudio
```

#### Compiler flags
When importing the CCS project, some systems may cut out some of the compiler flags needed to build this project without errors. To make sure all your compiler flags are set correctly, have a look at [this list](https://github.com/phdschooltpc/msp430-thyroid/blob/master/list_of_compiler_flags.md) and compare it with your project's compiler settings in `Project > Properties > CCS Build`.
