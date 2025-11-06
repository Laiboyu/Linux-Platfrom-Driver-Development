.PHONY:clean
ifeq ($(KERNELRELEASE),)
    # Assume the source tree is where the running kernel was built
    # You should set KERNELDIR in the environment if it's elsewhere
    #/home/joey/orangepi-build/kernel/orange-pi-5.4-sun50iw9 /home/joey/orangepi-build/kernel/orange-pi-4.9-sun50iw9

    KERNELDIR ?= /home/joey/orangepi-build/kernel/orange-pi-5.4-sun50iw9
    # The current directory is passed to sub-makes as argument
    PWD := $(shell pwd)
modules:
	$(MAKE) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C $(KERNELDIR) M=$(PWD) modules
else
    obj-m := taskletExm.o
endif
	
clean:
	rm -f *.o *.exe
	rm -f *.ko *.cmd
	rm -f *.mod.*
	rm -f *.mod
	rm -f [mM]odule*
 