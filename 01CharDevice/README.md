## Create a character device for both WSL2 (Host) and RaspberryPi (Cross-Compilation)

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
