MODULE_NAME = matrix_module

obj-m := $(MODULE_NAME).o

KERN_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

CC := gcc

test.o: test.c
	$(CC) test.c -o test.o

all:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) clean

install: all
	sudo insmod $(MODULE_NAME).ko

remove:
	sudo rmmod $(MODULE_NAME)

check:
	lsmod | grep $(MODULE_NAME)

test: test.o
	./test.o
