
# Yocto and Raspberry Pi Project Setup (WSL2 on Windows 10)

## 1. Install Required Packages
```bash
# Update package repositories
sudo apt update

# Install packages required for Yocto build
sudo apt install -y gawk wget git diffstat unzip texinfo gcc build-essential \
chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils \
iputils-ping python3-git python3-jinja2 python3-subunit zstd liblz4-tool file \
locales libacl1 libssl-dev libncurses5-dev u-boot-tools

# Generate locale
sudo locale-gen en_US.UTF-8
```

## 2. Download the Poky Sources
```bash
git clone git://git.yoctoproject.org/poky (this really takes time)
```

## 3. Checkout the Latest Branch/Release
```bash
cd poky
git checkout kirkstone
```

## 4. Add a Package to the Root File System
Edit your `local.conf` file and add the desired package:

```plaintext
IMAGE_INSTALL += "recipe-name"
```
For example, to add `usbutils` for `lsusb`, use:

```plaintext
IMAGE_INSTALL += "usbutils"
```
Alternatively, you can append it:

```plaintext
IMAGE_INSTALL_append = " usbutils"
```

## 5. Where to Get Other Layers
- [OpenEmbedded Layer Index](https://layers.openembedded.org/layerindex/branch/master/layers/)
- [Yocto Project Layers](https://www.yoctoproject.org/software-overview/layers/)

## 6. Poky Source Tree Structure
- **bitbake**: Contains all Python scripts used by the `bitbake` command.
- **documentation**: Source files for Yocto Project documentation.
- **meta**: Contains the core metadata.
- **meta-poky**: Configuration for the Poky reference distribution.
- **meta-skeleton**: Template recipes for BSP and kernel development.
- **meta-yocto-bsp**: Maintains BSPs for the Beaglebone, EdgeRouter, and generic IA machines.
- **scripts**: Scripts for environment setup, development tools, and flashing generated images.
- **LICENSE**: The license under which Poky is distributed (GPLv2 and MIT).

## 7. Raspberry Pi Layer Setup
- Clone the Raspberry Pi meta-layer:
```bash
git clone git://git.yoctoproject.org/meta-raspberrypi
cd meta-raspberrypi
git checkout kirkstone
```
### Dependencies
- **meta-openembedded**: Required dependencies for the Raspberry Pi layer.
```bash
git clone git://git.openembedded.org/meta-openembedded
cd meta-openembedded
git checkout kirkstone
```

### Layers Needed:
- `meta-oe`
- `meta-multimedia`
- `meta-networking`
- `meta-python`

## 8. Directory Structure After Cloning
```plaintext
~/embd_linux/
├── poky
├── meta-raspberrypi
└── meta-openembedded
```

## 9. Building the Image
1. Run the environment script to set up the Yocto Environment and create the build directory:
```bash
source poky/oe-init-build-env build_pi
```

2. Add `meta-openembedded` layers (meta-oe, meta-multimedia, meta-networking, meta-python) and the `meta-raspberrypi` layer to `build_pi/conf/bblayers.conf`.
```bash
  BBLAYERS ?= " \
  ${TOPDIR}/../poky/meta \
  ${TOPDIR}/../poky/meta-poky \
  ${TOPDIR}/../poky/meta-yocto-bsp \
  ${TOPDIR}/../meta-openembedded/meta-oe \
  ${TOPDIR}/../meta-openembedded/meta-multimedia \
  ${TOPDIR}/../meta-openembedded/meta-networking \
  ${TOPDIR}/../meta-openembedded/meta-python \
  ${TOPDIR}/../meta-raspberrypi \
  "
```

3. Configure local.conf for Raspberry Pi 3: Edit the build_pi/conf/local.conf file to set up the Raspberry Pi and enable UART, SPI, I2C, SSH, and Ethernet:
```plaintext
# Set the target machine
MACHINE = "raspberrypi3"

# Enable UART
ENABLE_UART = "1"

# Enable SSH server
EXTRA_IMAGE_FEATURES += " ssh-server-openssh"

# Include Ethernet, SPI, and I2C tools
IMAGE_INSTALL += " i2c-tools spi-tools python3-serial dhcp-client iproute2"

# Add support for systemd
DISTRO_FEATURES += " systemd"
VIRTUAL-RUNTIME_init_manager = "systemd"

BB_GIT_CLONE_FOR_SRC_URI = "1"
BB_FETCH_TIMEOUT = "60000"
GIT_CONFIG[user.timeout] = "60000"
BB_GIT_SHALLOW = "1"
GIT_SHALLOW_pn-linux-raspberrypi = "1"
BB_NUMBER_THREADS = "8"
PARALLEL_MAKE = "-j8"
```

4. To see available images:
```bash
ls ../meta-raspberrypi/recipes-*/images/
```

## 11. Images Overview
- **rpi-hwup-image.bb**: Based on `core-image-minimal`.
- **rpi-basic-image.bb**: Based on `rpi-hwup-image.bb` with added features (e.g., splash screen).
- **rpi-test-image.bb**: Based on `rpi-basic-image.bb`, includes packages from `meta-raspberrypi`.

## 12. Build Image:
```bash
bitbake core-image-minimal
```

## 13. Raspberry Pi Information
- **Broadcom BCM2837**
  - **CPU**: 1.2GHz 64-bit quad-core ARMv8 Cortex-A53
  - **GPU**: Broadcom VideoCore IV
  - **SDRAM**: 1024 MiB

## 14. Booting Sequence in Raspberry Pi
1. **GPU Core**: First stage bootloader, stored in ROM on the SoC.
2. **bootcode.bin**
3. **start.elf**
4. **config.txt**
5. **cmdline.txt**
6. **kernel7.img**

## 15. Image Location
The built images can be found at:

```plaintext
tmp/deploy/images/raspberrypi3/core-image-minimal-raspberrypi3-20241004105158.rootfs.wic.bz2
```

## 16. IMAGE_ROOTFS_EXTRA_SPACE
This variable adds extra free space to the root filesystem image. The value is specified in kilobytes.

### Default Value: 0
For example, to add an additional 4 GB of space:

```plaintext
IMAGE_ROOTFS_EXTRA_SPACE = "4194304"
```
