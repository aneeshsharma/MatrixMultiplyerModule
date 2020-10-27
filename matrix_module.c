#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
// Use General Public License
MODULE_LICENSE("GPL");

// Name of the device to be registered
const char* DEVICE_NAME = "matrix-multiplyer";

ssize_t matrix_module_read(struct file*, char*, size_t, loff_t*);
ssize_t matrix_module_write(struct file*, const char*, size_t, loff_t*);

static const char msg[] = "Kernel says hello!";
static const ssize_t msg_len = sizeof(msg);

static char write_buffer[1024];

static struct file_operations matrix_fops = {
	.owner = THIS_MODULE,
	.read = matrix_module_read, 
	.write = matrix_module_write,
};

// Stores the major number for the device
static int device_major_number = 0;

int matrix_module_init(void) {
	int result;
	printk(KERN_NOTICE "Initialising deivce - %s function\n", __FUNCTION__);
	
	// Resgiter a new character device
	result = register_chrdev(0, 
							DEVICE_NAME, 
							&matrix_fops);
	// if result is -ve, an error has occured
	if (result < 0) {
		printk(KERN_WARNING "Error registering device. Code = %d\n", result);
		return result;
	}

	// if result is positive, it is the registered major device number
	device_major_number = result;
	printk(KERN_NOTICE "Registered device with major number - %d\n", device_major_number);
	return 0;
}

ssize_t matrix_module_read(struct file* filep, char* buffer, size_t len, loff_t* offset) {
	printk(KERN_NOTICE "Called function - %s\n", __FUNCTION__);

	// return 0 if offset is beyond the message length
	if (*offset >= msg_len)
		return 0;
	
	// if end point is beyond message length, change length
	if (*offset + len > msg_len) {
		len = msg_len - *offset;
	}

	// copy message data to user buffer address
	if (copy_to_user(buffer, msg + *offset, len)) {
		printk(KERN_ALERT "Error writing to use buffer\n");
		return -EFAULT;
	}
	*offset += len;
	return len;
}

ssize_t matrix_module_write(struct file* filep, const char* user_buffer, size_t len, loff_t* offset) {
	int i;
	if (*offset >= len)
		return 0;
	if (copy_from_user(write_buffer, user_buffer + *offset, len)) {
		printk(KERN_ALERT "Error copying from user buffer\n");
		return -EFAULT;
	}
	printk(KERN_NOTICE "Received string - ");
	for (i = 0; i < len; i++) {
		printk(KERN_NOTICE "%c", write_buffer[i]);
	}
	printk(KERN_NOTICE "\n");
	*offset += len;
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
