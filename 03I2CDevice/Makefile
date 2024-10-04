# Specify the kernel source directory
PI_KDIR := /home/kkumar/embd_linux/build_pi/tmp/work/raspberrypi3-poky-linux-gnueabi/linux-raspberrypi/1_5.15.92+gitAUTOINC+509f4b9d68_14b35093ca-r0/linux-raspberrypi3-standard-build
PC_KDIR := /lib/modules/$(shell uname -r)/build

# Set the name of the module
obj-m := i2c_device.o

# Check for the TARGET argument (default is PC)
TARGET ?= PC

# Set appropriate values based on the target platform
ifeq ($(TARGET), RPI)
	KDIR := $(PI_KDIR)
	ARCH := arm
else
	KDIR := $(PC_KDIR)
	ARCH := x86_64
	CROSS_COMPILE :=
endif

# Default target to build the kernel module
all:
	@echo "Building kernel module for $(TARGET) ..."
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules

# Clean target to remove build artifacts
clean:
	@echo "Cleaning build artifacts for $(TARGET) ..."
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) clean

# Print configuration information for debugging
info:
	@echo "Build Information:"
	@echo "Target Platform: $(TARGET)"
	@echo "Kernel Directory: $(KDIR)"
	@echo "Architecture: $(ARCH)"
	@echo "Cross Compiler Prefix: $(CROSS_COMPILE)"
