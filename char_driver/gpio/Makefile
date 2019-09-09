# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)

obj-m := hello.o

# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else

#
#KDIR := /home/v/linux-4.19.57
KDIR :=/home/v/work/linux
default:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-

clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers  modul*

endif
