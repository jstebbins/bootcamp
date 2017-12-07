obj-m += accel.o
KDIR := ~/kernel
PWD := $(shell pwd)
CROSS := arm-eabi-

accel:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CROSS) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
