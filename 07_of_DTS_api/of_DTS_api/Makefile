
ifeq ($(KERNELRELEASE),)
    # Assume the source tree is where the running kernel was built
    # You should set KERNELDIR in the environment if it's elsewhere
    KERNELDIR ?= /home/cyh/orangepi-build/kernel/orange-pi-5.4-sun50iw9
    # The current directory is passed to sub-makes as argument
    PWD := $(shell pwd)
modules:
	$(MAKE) ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- -C $(KERNELDIR) M=$(PWD) modules
else
    obj-m := H1.o
endif
	
clean:
	rm -f *.o
	rm -f *.ko *.cmd
	rm -f H1.mod.*
	rm -f [mM]odule*
	rm -f .H1.*




