
ifneq ($(KERNELRELEASE),)

include $(src)/.config

microdia-objs := microdia-usb.o microdia-v4l.o microdia-sysfs.o
microdia-objs += microdia-dev.o microdia-decoder.o microdia-queue.o
microdia-objs += sn9c20x.o mt9vx11.o ov7670.o ov965x.o ov7660.o

ifeq ($(CONFIG_MICRODIA_DEBUGFS),y)
microdia-objs += microdia-debugfs.o
EXTRA_CFLAGS = -DCONFIG_MICRODIA_DEBUGFS
endif

obj-$(CONFIG_MICRODIA) += microdia.o

else

KVER=$(shell uname -r)
KSRC=/lib/modules/$(KVER)/build

all: driver ctags

doc:
	@echo "Generating document with Doxygen..."
	@doxygen Doxyfile

driver:
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD) modules

ctags:
	@ctags -R

clean:
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD) clean
	@rm -f Module.symvers Module.markers modules.order tags

cleandoc:
	@echo "Removing documentation generated by Doxygen..."
	@rm -Rf ./doc

insmod:
	@/sbin/insmod microdia.ko

rmmod:
	@/sbin/rmmod microdia

endif
