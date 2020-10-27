#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

// Use General Public License
MODULE_LICENSE("GPL");

// use a preset major number
#ifndef MAJOR_NUMBER
	#define MAJOR_NUMBER 240
#endif

// Name of the device to be registered
const char* DEVICE_NAME = "matrix-multiplyer";

ssize_t matrix_module_read(struct file*, char*, size_t, loff_t*);
ssize_t matrix_module_write(struct file*, const char*, size_t, loff_t*);
void calculateOutput(void);

static const char msg[] = "Kernel says hello!";
static const ssize_t msg_len = sizeof(msg);

// reserved memory to store the output
static int output[100][100];

static char output_string[1024];
static int output_len = 0;

// reserved memory to store the input values
static int input1[100][100];
static int input2[100][100];

// the dimension of input and output square matrices
// dimO is set to -1 if there's an error in multiplication
// or if the input format is incorrect
static int dimI;
static int dimO;

static char write_buffer[1024];

static struct file_operations matrix_fops = {
	.owner = THIS_MODULE,
	.read = matrix_module_read, 
	.write = matrix_module_write,
};

// Stores the major number for the device
static int device_major_number = 0;

int is_digit(char x) {
	return (x <= '9') && (x >= '0');
}

int matrix_module_init(void) {
	int result;
	printk(KERN_NOTICE "Initialising deivce - %s function\n", __FUNCTION__);
	
	// Resgiter a new character device
	result = register_chrdev(MAJOR_NUMBER, 
							DEVICE_NAME, 
							&matrix_fops);
	// if result is -ve, an error has occured
	if (result != 0) {
		printk(KERN_WARNING "Error registering device. Code = %d\n", result);
		return result;
	}

	// set major number
	device_major_number = MAJOR_NUMBER;
	printk(KERN_NOTICE "Registered device with major number - %d\n", device_major_number);
	return 0;
}

// read integers from a string
// returns 0 if successful and -1 on error
int readNextInt(char* string, int* index, int* result) {
	char c;
	int num;
	num = 0;
	while(string[*index] && (*index) < 1024) {
		c = string[*index];
		*index += 1;
		if (is_digit(c)) {
			num *= 10;
			num += (int) c - '0';
		} else if (c == ' ') {
			printk(KERN_NOTICE "%d\n", num);
			*result = num;
			return 0;
		} else {
			return -1;
		}
	}
	*result = num;
	return 0;
}

// write integer value to string
int writeInt(char* string, int *index, int number) {
	int digit, new, len, i;
	if (number < 0) {
		number = -number;
		string[*index] = '-';
		*index += 1;
	}
	
	// reverse the digits first
	new = 0;
	len = 0;
	if (!number)
		len = 1;
	while (number) {
		new *= 10;
		new += number % 10;
		number /= 10;
		len++;
	}
	number = new;

	for (i = 0; i < len; i++) {
		digit = number % 10;
		string[*index] = (char) digit + '0';
		number /= 10;
		*index += 1;
	}
	return 0;
}

void getOutputString(void) {
	int i, j;
	int index = 0;
	output_string[index] = 'O';
	index++;
	writeInt(output_string, &index, dimO);
	for (i = 0; i < dimO; i++) {
		for(j = 0; j < dimO; j++) {
			output_string[index] = ' ';
			index++;
			writeInt(output_string, &index, output[i][j]);
		}
	}
	// end with null character
	output_string[index] = 0;
	output_len = index + 1;
}

// to read the input matrices and calculate the output
void calculateOutput(void) {
	int index;
	int i, j, k, size;
	int sum;
	char first = write_buffer[0];
	if (first != 'I') {
		dimO = -1;
		return;
	}
	index = 1;
	if (readNextInt(write_buffer, &index, &size) == -1) {
		printk(KERN_NOTICE "Error reading input\n");
		dimO = -1;
		return;
	}
	printk(KERN_NOTICE "Input matrix size - %d\n", size);
	dimO = size;
	dimI = size;

	// input matrix 1
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			if (readNextInt(write_buffer, &index, &input1[i][j]) == -1) {
				printk(KERN_NOTICE "Error reading input\n");
				dimO = -1;
				return;
			}
		}
	}

	// input matrix 2
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			if (readNextInt(write_buffer, &index, &input2[i][j]) == -1) {
				printk(KERN_NOTICE "Error reading input\n");
				dimO = -1;
				return;
			}
		}
	}

	// multiply the 2 matrices
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			sum = 0;
			for (k = 0; k < size; k++) {
				sum += input1[i][k] * input2[k][j];
			}
			output[i][j] = sum;
		}
	}

	getOutputString();
	return;
}

ssize_t matrix_module_read(struct file* filep, char* buffer, size_t len, loff_t* offset) {
	printk(KERN_NOTICE "Called function - %s\n", __FUNCTION__);

	// return 0 if offset is beyond the message length
	if (*offset >= output_len)
		return 0;
	
	// if end point is beyond message length, change length
	if (*offset + len > output_len) {
		len = output_len - *offset;
	}

	// copy message data to user buffer address
	if (copy_to_user(buffer, output_string + *offset, len)) {
		printk(KERN_ALERT "Error writing to use buffer\n");
		return -EFAULT;
	}
	*offset += len;
	return len;
}

ssize_t matrix_module_write(struct file* filep, const char* user_buffer, size_t len, loff_t* offset) {
	if (*offset >= len)
		return 0;
	if (copy_from_user(write_buffer, user_buffer + *offset, len)) {
		printk(KERN_ALERT "Error copying from user buffer\n");
		return -EFAULT;
	}
	*offset += len;
	printk(KERN_NOTICE "Input length - %ld\n", len);
	// end with null character
	write_buffer[*offset] = 0;
	calculateOutput();

	return len;
}

void matrix_module_exit(void) {
	printk(KERN_NOTICE "Exit - %s function\n", __FUNCTION__);
	printk(KERN_NOTICE "Unregistering device with major number - %d\n", device_major_number);
	if (device_major_number != 0) {
		unregister_chrdev(device_major_number, DEVICE_NAME);
	}
}

module_init(matrix_module_init);
module_exit(matrix_module_exit);
