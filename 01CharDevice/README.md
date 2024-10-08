## Create a character device for both WSL2 and RaspberryPi (Cross-Compilation)

```bash
kkumar@DESKTOP-NK9HSKR:/mnt/c/Users/kumar$ uname -a
Linux DESKTOP-NK9HSKR 5.15.153.1-microsoft-standard-WSL2+ #2 SMP Thu Oct 3 10:36:07 CEST 2024 x86_64 x86_64 x86_64 GNU/Linux
```

### Step 1: Compile for host
```bash
make
```
```plaintext
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ make
make -C /lib/modules/5.15.153.1-microsoft-standard-WSL2+/build M=/home/kkumar/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice modules
make[1]: Entering directory '/home/kkumar/WSL2-Linux-Kernel'
  CC [M]  /home/kkumar/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice/char_device.o
  MODPOST /home/kkumar/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice/Module.symvers
  CC [M]  /home/kkumar/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice/char_device.mod.o
  LD [M]  /home/kkumar/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice/char_device.ko
make[1]: Leaving directory '/home/kkumar/WSL2-Linux-Kernel'
```

### Step 2: load module
```plaintext
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ sudo insmod char_device.ko
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ dmesg | tail
....
....
[13253.248521] executing ModuleCharacterDeviceInit
[13253.248525] ModuleCharacterDeviceInit device number <major>:<minor> = 240:0
[13253.248673] ModuleCharacterDeviceInit device created successfully..  
```

### Step 3: Check device status
```plaintext
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ cat /sys/class/pdevclass/pdev/dev
240:0
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ cat /sys/class/pdevclass/pdev/uevent 
MAJOR=240
MINOR=0  
DEVNAME=pdev
```

### Step 3: remove module
```plaintext
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ sudo rmmod char_device
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ dmesg | tail
....
....
[13426.935342] executing ModuleCharacterDeviceExit
[13426.935516] ModuleCharacterDeviceExit device cleaned up successfully..
```

### Step 4: Set device permissions (if needed)
```bash
sudo chmod 666 /dev/pdev
sudo chown kkumar:kkumar /dev/pdev
```

```plaintext
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ sudo insmod char_device.ko
[sudo] password for kkumar: 
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ dmesg | grep SINGLE_CHAR_DEVICE
[16013.546055] SINGLE_CHAR_DEVICE: executing ModuleCharacterDeviceInit
[16013.546061] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device number <major>:<minor> = 240:0
[16013.546195] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device created successfully..
[16139.154948] SINGLE_CHAR_DEVICE: executing _open
[16139.154956] SINGLE_CHAR_DEVICE: executing _write, requested 19 bytes
[16139.154981] SINGLE_CHAR_DEVICE: executing _release
[16318.707585] SINGLE_CHAR_DEVICE: executing _open
[16318.707595] SINGLE_CHAR_DEVICE: executing _read, requested 131072 bytes
[16318.707618] SINGLE_CHAR_DEVICE: executing _read, requested 131072 bytes
[16318.707627] SINGLE_CHAR_DEVICE: executing _release
[16487.121518] SINGLE_CHAR_DEVICE: executing ModuleCharacterDeviceExit
[16487.121672] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceExit device cleaned up successfully..
[17636.843632] SINGLE_CHAR_DEVICE: executing ModuleCharacterDeviceInit
[17636.843636] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device number <major>:<minor> = 240:0
[17636.843774] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device created successfully..
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ sudo chown kkumar:kkumar /dev/pdev
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ sudo chmod 666 /dev/pdev
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ echo "test bytes written" > /dev/pdev
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ dmesg | grep SINGLE_CHAR_DEVICE
[16013.546055] SINGLE_CHAR_DEVICE: executing ModuleCharacterDeviceInit
[16013.546061] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device number <major>:<minor> = 240:0
[16013.546195] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device created successfully..
[16139.154948] SINGLE_CHAR_DEVICE: executing _open
[16139.154956] SINGLE_CHAR_DEVICE: executing _write, requested 19 bytes
[16139.154981] SINGLE_CHAR_DEVICE: executing _release
[16318.707585] SINGLE_CHAR_DEVICE: executing _open
[16318.707595] SINGLE_CHAR_DEVICE: executing _read, requested 131072 bytes
[16318.707618] SINGLE_CHAR_DEVICE: executing _read, requested 131072 bytes
[16318.707627] SINGLE_CHAR_DEVICE: executing _release
[16487.121518] SINGLE_CHAR_DEVICE: executing ModuleCharacterDeviceExit
[16487.121672] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceExit device cleaned up successfully..
[17636.843632] SINGLE_CHAR_DEVICE: executing ModuleCharacterDeviceInit
[17636.843636] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device number <major>:<minor> = 240:0
[17636.843774] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device created successfully..
[17677.059988] SINGLE_CHAR_DEVICE: executing _open
[17677.060000] SINGLE_CHAR_DEVICE: executing _write, requested 19 bytes
[17677.060027] SINGLE_CHAR_DEVICE: executing _release
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ cat /dev/pdev
test bytes written
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ dmesg | grep SINGLE_CHAR_DEVICE
[16013.546055] SINGLE_CHAR_DEVICE: executing ModuleCharacterDeviceInit
[16013.546061] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device number <major>:<minor> = 240:0
[16013.546195] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device created successfully..
[16139.154948] SINGLE_CHAR_DEVICE: executing _open
[16139.154956] SINGLE_CHAR_DEVICE: executing _write, requested 19 bytes
[16139.154981] SINGLE_CHAR_DEVICE: executing _release
[16318.707585] SINGLE_CHAR_DEVICE: executing _open
[16318.707595] SINGLE_CHAR_DEVICE: executing _read, requested 131072 bytes
[16318.707618] SINGLE_CHAR_DEVICE: executing _read, requested 131072 bytes
[16318.707627] SINGLE_CHAR_DEVICE: executing _release
[16487.121518] SINGLE_CHAR_DEVICE: executing ModuleCharacterDeviceExit
[16487.121672] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceExit device cleaned up successfully..
[17636.843632] SINGLE_CHAR_DEVICE: executing ModuleCharacterDeviceInit
[17636.843636] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device number <major>:<minor> = 240:0
[17636.843774] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device created successfully..
[17677.059988] SINGLE_CHAR_DEVICE: executing _open
[17677.060000] SINGLE_CHAR_DEVICE: executing _write, requested 19 bytes
[17677.060027] SINGLE_CHAR_DEVICE: executing _release
[17693.518795] SINGLE_CHAR_DEVICE: executing _open
[17693.518805] SINGLE_CHAR_DEVICE: executing _read, requested 131072 bytes
[17693.518828] SINGLE_CHAR_DEVICE: executing _read, requested 131072 bytes
[17693.518837] SINGLE_CHAR_DEVICE: executing _release
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ sudo rmmod char_device
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/01CharDevice$ dmesg | grep SINGLE_CHAR_DEVICE
[16013.546055] SINGLE_CHAR_DEVICE: executing ModuleCharacterDeviceInit
[16013.546061] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device number <major>:<minor> = 240:0
[16013.546195] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device created successfully..
[16139.154948] SINGLE_CHAR_DEVICE: executing _open
[16139.154956] SINGLE_CHAR_DEVICE: executing _write, requested 19 bytes
[16139.154981] SINGLE_CHAR_DEVICE: executing _release
[16318.707585] SINGLE_CHAR_DEVICE: executing _open
[16318.707595] SINGLE_CHAR_DEVICE: executing _read, requested 131072 bytes
[16318.707618] SINGLE_CHAR_DEVICE: executing _read, requested 131072 bytes
[16318.707627] SINGLE_CHAR_DEVICE: executing _release
[16487.121518] SINGLE_CHAR_DEVICE: executing ModuleCharacterDeviceExit
[16487.121672] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceExit device cleaned up successfully..
[17636.843632] SINGLE_CHAR_DEVICE: executing ModuleCharacterDeviceInit
[17636.843636] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device number <major>:<minor> = 240:0
[17636.843774] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device created successfully..
[17677.059988] SINGLE_CHAR_DEVICE: executing _open
[17677.060000] SINGLE_CHAR_DEVICE: executing _write, requested 19 bytes
[17677.060027] SINGLE_CHAR_DEVICE: executing _release
[17693.518795] SINGLE_CHAR_DEVICE: executing _open
[17693.518805] SINGLE_CHAR_DEVICE: executing _read, requested 131072 bytes
[17693.518828] SINGLE_CHAR_DEVICE: executing _read, requested 131072 bytes
[17693.518837] SINGLE_CHAR_DEVICE: executing _release
[17713.568753] SINGLE_CHAR_DEVICE: executing ModuleCharacterDeviceExit
[17713.568999] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceExit device cleaned up successfully..
```

## Let's prepare enviroment for RaspberryPi (Cross-Compilation) on WSL2 (Host)

[RaspberryPi build env setup on WSL2](https://github.com/Kishwar/RaspberryPi_Linux_Drivers_Development/blob/main/README.md)

### 1. Build the Yocto Toolchain for the Raspberry Pi (if not already built)
```bash
bitbake meta-toolchain
```

### 2. Source the Toolchain Environment Script
After building the toolchain, Yocto will generate a toolchain setup script (e.g., environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi). This script sets up the necessary cross-compilation variables.
```bash
source tmp/sysroots/raspberrypi3/imgdata/core-image-minimal.env
```

### 3. Get the RaspberryPi Kernel Headers
You need the kernel headers for your specific RaspberryPi kernel version. Use the Yocto build system to extract and set up the headers.
```bash
bitbake virtual/kernel -c devshell
```
Above command will open devshell. You will need to build LKM inside the window.

![devshell](make_make_clean_raspberrypi_cross_compilation.png)

### 4. Load and output from RaspberryPi
```bash
PS X:\home\kkumar\embd_linux\RaspberryPi_Linux_Drivers_Development\01CharDevice> scp char_device.ko root@192.168.178.98:/home/root/chardevice/char_device.ko    100%   10KB 401.3KB/s   00:00 
```
```plaintext
root@raspberrypi3:~/chardevice# insmod char_device.ko
root@raspberrypi3:~/chardevice# dmesg | tail
....
....
[ 7347.081745] SINGLE_CHAR_DEVICE: executing ModuleCharacterDeviceInit
[ 7347.088172] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device number <major>:<minor> = 241:0
[ 7347.099777] SINGLE_CHAR_DEVICE: ModuleCharacterDeviceInit device created successfully..
root@raspberrypi3:~/chardevice# ls /dev
....
hwrng         pdev          shm           tty2          tty38         tty56         vcs1          watchdog
....
root@raspberrypi3:~/chardevice# echo "test values" > /dev/pdev
root@raspberrypi3:~/chardevice# dmesg | tail
....
[ 7881.766968] SINGLE_CHAR_DEVICE: executing _open
[ 7881.771721] SINGLE_CHAR_DEVICE: executing _write, requested 12 bytes
[ 7881.778597] SINGLE_CHAR_DEVICE: executing _release
root@raspberrypi3:~/chardevice# echo "test values write them again" > /dev/pdev
root@raspberrypi3:~/chardevice# dmesg | tail
....
[ 7909.892462] SINGLE_CHAR_DEVICE: executing _open
[ 7909.897226] SINGLE_CHAR_DEVICE: executing _write, requested 29 bytes
[ 7909.903701] SINGLE_CHAR_DEVICE: executing _release
```
