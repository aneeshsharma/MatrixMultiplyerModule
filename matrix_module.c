#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

int matrix_module_init(void) {
	printk(KERN_ALERT "Initialize - %s function\n", __FUNCTION__);
	return 0;
}

void matrix_module_exit(void) {
	printk(KERN_ALERT "Exit - %s function\n", __FUNCTION__);
}

module_init(matrix_module_init);
module_exit(matrix_module_exit);
