
# Yocto and Raspberry Pi Project Setup

## 1. Install Required Packages
```bash
sudo apt install gawk wget git diffstat unzip texinfo gcc build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 python3-subunit zstd liblz4-tool file locales libacl1
sudo locale-gen en_US.UTF-8
sudo apt-get install gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential chrpath socat cpio python2 python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev xterm
```

## 2. Download the Poky Sources
```bash
git clone git://git.yoctoproject.org/poky
```

## 3. Checkout the Latest Branch/Release
```bash
cd poky
git checkout zeus
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
```
### Dependencies
- **meta-openembedded**: Required dependencies for the Raspberry Pi layer.
```bash
git clone git://git.openembedded.org/meta-openembedded
```

### Layers Needed:
- `meta-oe`
- `meta-multimedia`
- `meta-networking`
- `meta-python`

## 8. Directory Structure After Cloning
```plaintext
meta-openembedded  meta-raspberrypi  poky
```

## 9. Building the Image
1. Run the environment script to set up the Yocto Environment and create the build directory:
```bash
source poky/oe-init-build-env build_pi
```

2. Add `meta-openembedded` layers (meta-oe, meta-multimedia, meta-networking, meta-python) and the `meta-raspberrypi` layer to `bblayers.conf`.

3. Set the MACHINE in `local.conf` to `raspberrypi3`:
```bash
echo 'MACHINE = "raspberrypi3"' >> conf/local.conf
```

4. To see available images:
```bash
ls ../meta-raspberrypi/recipes-*/images/
```

## 10. Images Overview
- **rpi-hwup-image.bb**: Based on `core-image-minimal`.
- **rpi-basic-image.bb**: Based on `rpi-hwup-image.bb` with added features (e.g., splash screen).
- **rpi-test-image.bb**: Based on `rpi-basic-image.bb`, includes packages from `meta-raspberrypi`.

## 11. Enabling UART
Raspberry Pi 3 does not have UART enabled by default. To enable it, add the following to `local.conf`:

```plaintext
ENABLE_UART = "1"
```

Build the image:

```bash
bitbake rpi-hwup-image
```

## 12. Raspberry Pi Information
- **Broadcom BCM2837**
  - **CPU**: 1.2GHz 64-bit quad-core ARMv8 Cortex-A53
  - **GPU**: Broadcom VideoCore IV
  - **SDRAM**: 1024 MiB

## 13. Booting Sequence in Raspberry Pi
1. **GPU Core**: First stage bootloader, stored in ROM on the SoC.
2. **bootcode.bin**
3. **start.elf**
4. **config.txt**
5. **cmdline.txt**
6. **kernel7.img**

## 14. Image Location
The built images can be found at:

```plaintext
tmp/deploy/images/raspberrypi3
```

## 15. IMAGE_ROOTFS_EXTRA_SPACE
This variable adds extra free space to the root filesystem image. The value is specified in kilobytes.

### Default Value: 0
For example, to add an additional 4 GB of space:

```plaintext
IMAGE_ROOTFS_EXTRA_SPACE = "4194304"
```
