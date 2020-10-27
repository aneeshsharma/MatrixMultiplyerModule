# Matrix Multiplyer Module
This is a device driver written to simulate a matrix multiplying device. The matrix multiplication happens in the driver code itself and there's no actual device doing it.

This project is intended to learn how to write device drivers for linux.

## About the driver
This driver simulates a matrix multiplication device. The input is given as 2 square matrices of a given dimension and the ouput is the product of the 2 matrices with the same dimension as the input. The input is give to the device file at `/dev/matrix-multiplyer` and the output is also read from the same `/dev/matrix-multiplyer` device.
### Input format
The input starts with the character `I` followed by the dimension of the matrix (n). So, for a matrix of dimension 5x5, the input starts with `I5`.
This is followed by nxn numerical values which are the elemnts of the matrices. All elements are specifed in a row wise order, i.e. all elemnts of row 0 followed by all elements of row 1 and so on. This is followed by the elemnts of the second matrix in the same manner.

For example, consider the matrix -
```
1 2 3
1 2 3
1 2 3
```

For this matrix, the input format would be -
```
I3 1 2 3 1 2 3 1 2 3
```

### Output Format
The output format follows a similar format to input. The output starts with `O` followed by the dimenstion of the output (say n), followed by nxn numerical values representing the elemnts of the output matrix.
For example, consider the output-
```
O3 6 12 18 6 12 18 6 12 18
```

The matrix represented by this output will be -
```
6 12 18
6 12 18
6 12 18
```

## Prerequisites
Before installing and testing this driver module, make sure you have the following packages installed -
- `make` - the make utility
- `gcc` - the GNU Compiler Collection

## Usage
The project includes a `Makefile`. The operations on the `Makefile` are given below. You can use them to compile the module and install the module to your system. It also includes a test script that can be used to test the module-

### Build - `make`
To build the module, simply run `make`. This compiles the module using your current kernel.

### Install - `make install`
To install the module, run `make install`. This has a dependency to the target `all`. So, installing would automatically compile the module if not done already.
This will also create a new device file at `/dev/matrix-multiplyer`.
> Note: The driver uses the major number 240. So, make sure that is free or the install will fail

### Test - `make test`
To test the module, run `make test`. This will compile the `test.c` file and run the test. You need to run `make install` before this to install the module.
`test.c` tests the module against a 5x5 matrix and checks if the output matches with that of a regular matrix mutiplication function.

### Remove - `make remove`
To remove the module, run `make remove`. This will remove the module as well as delete the device file at `/dev/matrix-multiplyer`.

### Check - `make check`
To check for the module's install status, run `make check`. This will check if the module is installed or not.
