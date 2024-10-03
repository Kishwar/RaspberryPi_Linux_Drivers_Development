## PREPARE WSL2 KERNEL HEADERS (WSL2 KERNEL RUNNING ON WINDOWS 10)

### Why Linux Headers are Needed in WSL2
Kernel headers are required for compiling software that needs to interact with the kernel, such as kernel modules or drivers.
WSL2 uses a modified Linux kernel, which makes it difficult to install the standard Linux headers using typical package management tools like apt-get.
### Solution: How to Get Linux Headers on WSL2
Unfortunately, there is no direct way to install headers using the default Microsoft-provided WSL2 kernel. To obtain the appropriate headers, you need to compile a custom kernel for WSL2 with the correct configurations.

### Install build tools and libraries needed for compiling the kernel
```bash
sudo apt update
sudo apt install -y build-essential libncurses-dev flex bison libssl-dev bc libelf-dev libudev-dev libpci-dev libiberty-dev autoconf
```

### Step 1: Clone the WSL2 Kernel Repository
```bash
git clone https://github.com/microsoft/WSL2-Linux-Kernel.git
cd WSL2-Linux-Kernel
```

### Step 2: Check the Current Kernel Version in WSL2
```bash
uname -r
```

### Step 3: Select the Correct Branch for Your Kernel Version (5.15.153.1-microsoft-standard-WSL2)
```bash
git checkout linux-msft-wsl-5.15.y
```

### Step 4: Load the default WSL configuration
  #### Enable Loadable Module Support
  ```bash
  make KCONFIG_CONFIG=Microsoft/config-wsl menuconfig
  ```
  ```plaintext
  -> Enable Loadable module support  --->
    [*] Forced module loading
    [*] Module unloading
    [*] Forced module unloading
    [*] Module versioning support
    [*] Source checksum for all modules
  ```
 #### Enable Debug Information for Module Testing (Optional)
  ```plaintext
  -> Kernel hacking  --->
    [*] Compile the kernel with debug info
  ```
 #### Ensure that the configuration includes CONFIG_HEADERS for generating the kernel headers
 ```plaintext
 -> General Setup  --->
  <*> Kernel .config support
  [*] Enable access to .config through /proc/config.gz
 ```

  #### Manually Enable bzImage Support in the .config File
  ```bash
  nano Microsoft/config-wsl
  ```
  ```plaintext
  CONFIG_KERNEL_XZ=y
  CONFIG_KERNEL_BZIP2=y
  CONFIG_KERNEL_LZMA=y
  CONFIG_KERNEL_LZO=y
  CONFIG_KERNEL_LZ4=y
  CONFIG_KERNEL_ZSTD=y
  CONFIG_HAVE_KERNEL_BZIP2=y
  CONFIG_HAVE_KERNEL_XZ=y
  CONFIG_HAVE_KERNEL_LZMA=y
  CONFIG_HAVE_KERNEL_LZO=y
  CONFIG_HAVE_KERNEL_LZ4=y
  CONFIG_HAVE_KERNEL_ZSTD=y

  CONFIG_KERNEL_GZIP=y
  CONFIG_KERNEL_LZMA=y
  CONFIG_KERNEL_XZ=y
  CONFIG_KERNEL_ZSTD=y
  ```
  #### Search following after saving above file
  ```bash
  grep -i "CONFIG_64BIT" Microsoft/config-wsl
  grep -i "CONFIG_X86_64" Microsoft/config-ws;
  ```
  #### Copy the Default WSL2 Configuration
   ```bash
    cp Microsoft/config-wsl .config
   ```

### Step 5: Compile the WSL2 Kernel and Headers
 ```bash
  make clean
  make -j$(nproc)
 ```

### Step 6: Install Kernel Headers
  #### Install the kernel headers to a directory, such as /usr/local/include  
 ```bash
  sudo make headers_install
  sudo make headers_install INSTALL_HDR_PATH=/usr/src/linux-headers-$(uname -r)        <-- OR
  sudo ln -s /usr/src/linux-headers-$(uname -r) /lib/modules/$(uname -r)/build
 ```

### Step 7: Use the WSL2 Kernel in Your WSL Environment
  #### After building the custom kernel, you can configure WSL2 to use your new kernel
  ```bash
  cp arch/x86/boot/bzImage /mnt/c/WSL2Kernel/bzImage
  ```

### Step 8: Create or edit the .wslconfig file in your Windows home directory (C:\Users\<YourUsername>\.wslconfig)
  ```bash
  [wsl2]
  kernel=C:\\WSL2Kernel\\bzImage
  ```
### Step 9: Restart WSL2
```bash
wsl --shutdown
```

### Step 10: Check new kernel - it should have new date from build date
```bash
uname -a
```

### Step 11: See if step 6 is again needed here (Optional)

### Step 12: Build 00HelloWorld example
```bash
make
```
```plaintext
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/00helloWorld$ make
make -C /lib/modules/5.15.153.1-microsoft-standard-WSL2+/build M=/home/kkumar/embd_linux/RaspberryPi_Linux_Drivers_Development/00helloWorld modules
make[1]: Entering directory '/home/kkumar/WSL2-Linux-Kernel'
  CC [M]  /home/kkumar/embd_linux/RaspberryPi_Linux_Drivers_Development/00helloWorld/hello.o
  MODPOST /home/kkumar/embd_linux/RaspberryPi_Linux_Drivers_Development/00helloWorld/Module.symvers
  CC [M]  /home/kkumar/embd_linux/RaspberryPi_Linux_Drivers_Development/00helloWorld/hello.mod.o
  LD [M]  /home/kkumar/embd_linux/RaspberryPi_Linux_Drivers_Development/00helloWorld/hello.ko
make[1]: Leaving directory '/home/kkumar/WSL2-Linux-Kernel'
```

### Step 13: load module
```plaintext
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/00helloWorld$ sudo insmod hello.ko
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/00helloWorld$ dmesg | tail
....
....
[ 2443.989322] Hello, Kernel module (LKM) - KLM loaded on WSL2 Kernel --- Wooohooo ---
```

### Step 14: remove module
```plaintext
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/00helloWorld$ sudo rmmod hello
kkumar@DESKTOP-NK9HSKR:~/embd_linux/RaspberryPi_Linux_Drivers_Development/00helloWorld$ dmesg | tail
....
....
[ 2523.817586] Goodbye, Kernel module (LKM) -- KLM removed from WSL2 Kernle ----  Hurray!! ----
```
