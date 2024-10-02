# RaspberryPi_Linux_Drivers_Development

1. install packages:
    sudo apt install gawk wget git diffstat unzip texinfo gcc build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 python3-subunit zstd liblz4-tool file locales libacl1
	sudo locale-gen en_US.UTF-8
	sudo apt-get install gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential chrpath socat cpio python2 python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev xterm
	
2. download sources of poky
	kkumar@DESKTOP-NK9HSKR:~/embd_linux$ git clone git://git.yoctoproject.org/poky

3. Checkout the latest branch/release (zeus)  -> git checkout zeus 

4. ==== To add a particular package in your root file system =====
	Open your local.conf file and add the recipe name below
	IMAGE_INSTALL += "recipe-name"
	E.g. IMAGE_INSTALL += "usbutils" for lsusb
	or IMAGE_INSTALL_append = " usbutils"

5. ==== Where to get other layers ==== 
	https://layers.openembedded.org/layerindex/branch/master/layers/
	https://www.yoctoproject.org/software-overview/layers/

6. Poky source tree
	bitbake		-	Holds all Python scripts used by the bitbake command
			bitbake/bin is placed into the PATH environmental variable so bitbake can be found
	documentation   -       All documentation sources for the Yocto Project documentation
			Can be used to generate nice PDFs
	meta		-	Contains the oe-core metadata
			
	meta-poky	-	Holds the configuration for the Poky reference distribution
			local.conf.sample, bblayers.conf.sample are present here
	meta-skeleton	- 	Contains template recipes for BSP and kernel development
	meta-yocto-bsp  -	Maintains several BSPs such as the Beaglebone, EdgeRouter, 
			and generic versions of both 32-bit and 64-bit IA machines.
	scripts		-	Contains scripts used to set up the environment, development tools,
			and tools to flash the generated images on the target.
	LICENSE		-	The license under which Poky is distributed (a mix of GPLv2 and MIT).

7. RaspberryPi
	git://git.yoctoproject.org/meta-raspberrypi
	=== Dependencies ====
	CLONE 	: git clone git://git.openembedded.org/meta-openembedded (raspberrypi layer depends on it)
	CLONE   : git clone git://git.yoctoproject.org/meta-raspberrypi
	Layers	: meta-oe, meta-multimedia, meta-networking, meta-python

8. After above step, folders should be
	meta-openembedded  meta-raspberrypi  poky

9. Let's start building
	1. Run the environment script to setup the Yocto Environment and create build directory
		source poky/oe-init-build-env build_pi

	2. Add meta-openembedded layers ( meta-oe, meta-multimedia, meta-networking, meta-python) and meta-raspberrypi layer to bblayers.conf
	
	3. Set the MACHINE in local.conf to "raspberrypi3".
		echo 'MACHINE = "raspberrypi3"' >> conf/local.conf
	
	4. Final step is to build the image. To find out the available images:
		ls ../meta-raspberrypi/recipes-*/images/

		
10. Images
	rpi-hwup-image.bb: This is an image based on core-image-minimal
	rpi-basic-image.bb: This is an image based on rpi-hwup-image.bb, with some added features (a splash screen)
	rpi-test-image.bb: This is an image based on rpi-basic-image.bb, which includes some packages present in meta-raspberrypi

11. Enabling UART
	RaspberryPi 3 does not have the UART enabled by default because this needs a fixed core frequency and enable_uart wil set it to the minimum
	To enable it, set the following in local.conf
		ENABLE_UART = "1"

	$ bitbake rpi-hwup-image

12. RaspberryPi Info:
	Broadcom BCM2837:
	CPU 	-	1.2GHz 64-bit quad-core ARMv8 Cortex-A53
	GPU 	-	Broadcom VideoCore IV
	SDRAM	-	1024 MiB

13. Booting sequence in RaspberryPi
	1. GPU Core
		first stage bootloader, which is stored in ROM on the SoC
	2. bootcode.bin
	3. start.elf
	4. config.txt
	5. cmdline.txt
	6. kernel7.img
	
14. Images are present at Images are present in tmp/deploy/images/raspberrypi3

15. IMAGE_ROOTFS_EXTRA_SPACE
	Adds extra free space to the root filesystem image.
	The variable specifies the value in kilobytes
	Default value : 0
	For example, to add an additional 4 GB of space, set the variable to IMAGE_ROOTFS_EXTRA_SPACE = "4194304"

