MODULE_NAME = G19_code

obj-m := $(MODULE_NAME).o

KERN_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

CC := gcc

all:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) clean

install: all
	sudo insmod $(MODULE_NAME).ko
	sudo mknod -m 666 /dev/matrix-multiplyer c 240 0

remove:
	sudo rmmod $(MODULE_NAME)
	sudo rm /dev/matrix-multiplyer

check:
	lsmod | grep $(MODULE_NAME)

G19_test.o: G19_test.c
	$(CC) G19_test.c -o G19_test.o

test: G19_test.o
	./G19_test.o
