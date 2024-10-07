/************************************************************
 *  char_driver.c - Simple Linux Kernel Pseudo Character Driver
 *
 *  Description:
 *      This is a basic Linux kernel pseudo character device driver.
 *      The driver implements open, close, read, write, and lseek operations.
 *      It can be used as a template for developing more complex
 *      character drivers.
 *
 *  Functionality:
 *      - Registers a character device with the kernel.
 *      - Implements open, release (close), read, write, and lseek operations.
 *
 *  Usage:
 *      - To compile: `make`
 *      - To load: `sudo insmod char_driver.ko`
 *      - To create a device node: `sudo mknod /dev/simple_char c <major> 0`
 *      - To remove: `sudo rmmod char_driver`
 *
 *  License:
 *      This source code is licensed under the GPL License.
 *
 *  Author:
 *      Your Name (kumar.kishwar@gmail.com)
 *      Date: October 2024
 ************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/i2c.h>

/* meta information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kishwar Kumar");
MODULE_DESCRIPTION("This is a basic Linux kernel drivers for i2c connecting BMP280.");

static int32_t  read_temperature(void);

#define MODULE_NAME "SINGLE_CHAR_I2C_DEVICE"

/* lets store device number */
dev_t device_number;

/*cdev variable*/
struct cdev pcdev;

static struct i2c_adapter *bmp_i2c_adapter = NULL;
static struct i2c_client *bmp280_i2c_client = NULL;

/* Defines for device identification */ 
#define I2C_BUS_AVAILABLE	1	         	/* The I2C Bus available on the raspberry */
#define SLAVE_DEVICE_NAME	"BMP280"	  /* Device and Driver Name */
#define BMP280_SLAVE_ADDRESS	0x76		/* BMP280 I2C address */

/* Variables for temperature calculation */
int32_t dig_T1, dig_T2, dig_T3;

static const struct i2c_device_id bmp_id[] = {
  { SLAVE_DEVICE_NAME, 0 }, 
  { }
};

static struct i2c_driver bmp_driver = {
	.driver = {
		.name = SLAVE_DEVICE_NAME,
		.owner = THIS_MODULE
	}
};

static struct i2c_board_info bmp_i2c_board_info = {
	I2C_BOARD_INFO(SLAVE_DEVICE_NAME, BMP280_SLAVE_ADDRESS)
};

/*-------------------------------------------------------------------*/
/*define global functions*/
ssize_t _read(struct file *pfile, char __user *pbuff, size_t count, loff_t *poff)
{
  int32_t tmp;
  int to_copy, not_copied;

  pr_info("%s: executing %s, requested %zu bytes\n", MODULE_NAME, __func__, count);

  if(pbuff == NULL || poff == NULL)
  {
    pr_err("%s: %s invalid parameters.\n", MODULE_NAME, __func__);
    return -EINVAL;
  }

  /* get temporature */
  tmp = read_temperature();

  /* get size of data to copy */
  to_copy = min(count, sizeof(tmp));

  /*copy user data*/
  not_copied = copy_to_user(pbuff, &tmp, to_copy);

  if(not_copied != 0)
  {
    pr_err("%s: %s unable to copy data to user space.\n", MODULE_NAME, __func__);
    return EFAULT;
  }

  return to_copy - not_copied;
}

int _open(struct inode *node, struct file *pfile)
{
  pr_info("%s: executing %s\n", MODULE_NAME, __func__);
  return 0;
}

int _release(struct inode *pnode, struct file *pfile)
{
  pr_info("%s: executing %s\n", MODULE_NAME, __func__);
  return 0;
}

/*file operations of the driver*/
struct file_operations pcfops =
{
  .open    = _open,
  .read    = _read,
  .release = _release,
  .owner   = THIS_MODULE
};

struct class *pdclass;
struct device *pdevice;

/*-------------------------------------------------------------------*/
/*define static functions*/
/*-------------------------------------------------------------------*/
/**
 * @brief Read current temperature from BMP280 sensor
 * @return temperature in degree
 */
static int32_t  read_temperature(void) {
	int32_t  var1, var2;
	int32_t  raw_temp;
	int32_t  d1, d2, d3;

	/* Read Temperature */
	d1 = i2c_smbus_read_byte_data(bmp280_i2c_client, 0xFA);
	d2 = i2c_smbus_read_byte_data(bmp280_i2c_client, 0xFB);
	d3 = i2c_smbus_read_byte_data(bmp280_i2c_client, 0xFC);
	raw_temp = ((d1<<16) | (d2<<8) | d3) >> 4;

	/* Calculate temperature in degree */
	var1 = ((((raw_temp >> 3) - (dig_T1 << 1))) * (dig_T2)) >> 11;

	var2 = (((((raw_temp >> 4) - (dig_T1)) * ((raw_temp >> 4) - (dig_T1))) >> 12) * (dig_T3)) >> 14;
	return ((var1 + var2) *5 +128) >> 8;
}

/**
 * @brief this function is called, when the module is loaded into the kernel
 * @return 0 when module init OK, non-zero otherwise
 */
static int __init ModuleCharacterDeviceInit(void)
{
	u8 id;
  pr_info("%s: executing %s\n", MODULE_NAME, __func__);

  /*1. dynamically allocate a device number (creates device number)*/
  if(alloc_chrdev_region(&device_number, 0 /*first minor*/, 1 /*counts*/, "pdevice") < 0)
  {
    pr_err("%s: %s Failed to allocate a major number\n", MODULE_NAME, __func__);
    return -1;
  }

  pr_info("%s: %s device number <major>:<minor> = %d:%d\n", MODULE_NAME, __func__,
                                                      MAJOR(device_number),
                                                      MINOR(device_number));

  /*
    - The Virtual Filesystem (also known as Virtual Filesystem Switch or VFS) is a
      kernel software layer that handles all system calls related to a standard Unix
      filesystem. Its main strength is providing a common interface to several kinds
      of filesystems.

    - The Virtual File System (VFS) layer [1] provides a uniform interface for the
      kernel to deal with various I/O requests and specifies a standard interface
      that each file system must support.
  */
 
  /*2. create device class under /sys/class/ */
  pdclass = class_create(THIS_MODULE, "pdevclass");
  if (IS_ERR(pdclass))
  {
    unregister_chrdev_region(device_number, 1);
    pr_err("%s: %s Failed to register device class\n", MODULE_NAME, __func__);
    return PTR_ERR(pdclass);
  }

  /*3. Create the device file in /dev and set the permissions to 0666 */
  pdevice = device_create(pdclass, NULL, device_number, NULL, "pdev");
  if(pdevice == NULL)
  {
    class_destroy(pdclass);
    unregister_chrdev_region(device_number, 1);
    pr_err("%s: %s Failed to create the device\n", MODULE_NAME, __func__);
    return -1;
  }

  /*4. Initialize the character device and add it to the system*/
  cdev_init(&pcdev, &pcfops);
  pcdev.owner = THIS_MODULE;

  /*5. register a device (cdev structure) with VFS*/  
  if(cdev_add(&pcdev, device_number, 1) < 0)
  {
    device_destroy(pdclass, device_number);
    class_destroy(pdclass);
    unregister_chrdev_region(device_number, 1);
    pr_err("%s: %s Failed to add the cdev\n", MODULE_NAME, __func__);
    return -1;
  }

  bmp_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);
  if(bmp_i2c_adapter == NULL)
  {
    device_destroy(pdclass, device_number);
    class_destroy(pdclass);
    unregister_chrdev_region(device_number, 1);
    pr_info("%s: %s unable to get i2c adaptor...\n", MODULE_NAME, __func__);
    return -1;
  }

  bmp280_i2c_client = i2c_new_client_device(bmp_i2c_adapter, &bmp_i2c_board_info);
  if(bmp280_i2c_client == NULL)
  {
    device_destroy(pdclass, device_number);
    class_destroy(pdclass);
    unregister_chrdev_region(device_number, 1);
    pr_info("%s: %s unable to get i2c device...\n", MODULE_NAME, __func__);
    return -1;
  }

  if(i2c_add_driver(&bmp_driver) == -1)
  {
    device_destroy(pdclass, device_number);
    class_destroy(pdclass);
    unregister_chrdev_region(device_number, 1);
    i2c_unregister_device(bmp280_i2c_client);
    pr_info("%s: %s Can't add driver...\n", MODULE_NAME, __func__);
    return -1;
  }

  pr_info("%s: %s BMP280 Driver added!\n", MODULE_NAME, __func__);

  if (bmp280_i2c_client == NULL)
  {
    pr_err("%s: %s I2C client not initialized properly\n", MODULE_NAME, __func__);
    return -ENODEV;
  }

	/* Read Chip ID */
	id = i2c_smbus_read_byte_data(bmp280_i2c_client, 0xD0);
	pr_info("%s: %s ID: 0x%x\n", MODULE_NAME, __func__, id);

	/* Read Calibration Values */
  dig_T1 = i2c_smbus_read_word_data(bmp280_i2c_client, 0x88);
  dig_T2 = i2c_smbus_read_word_data(bmp280_i2c_client, 0x8a);
  dig_T3 = i2c_smbus_read_word_data(bmp280_i2c_client, 0x8c);

  if(dig_T2 > 32767) dig_T2 -= 65536;

	if(dig_T3 > 32767) dig_T3 -= 65536;

	/* Initialice the sensor */
  i2c_smbus_write_byte_data(bmp280_i2c_client, 0xf5, 5<<5);
  i2c_smbus_write_byte_data(bmp280_i2c_client, 0xf4, ((5<<5) | (5<<2) | (3<<0)));

  pr_info("%s: %s device created successfully..\n", MODULE_NAME, __func__);

  return 0;
}

/*
 * @brief this function is called, when the module is removed from the kernel
 */
static void __exit ModuleCharacterDeviceExit(void)
{
  pr_info("%s: executing %s\n", MODULE_NAME, __func__);
  
  /*cleanup task*/
  i2c_put_adapter(bmp_i2c_adapter);
  i2c_unregister_device(bmp280_i2c_client);
	i2c_del_driver(&bmp_driver);
  device_destroy(pdclass, device_number);
  class_destroy(pdclass);
  cdev_del(&pcdev);
  unregister_chrdev_region(device_number, 1);

  pr_info("%s: %s device cleaned up successfully..\n", MODULE_NAME, __func__);
}

module_init(ModuleCharacterDeviceInit);
module_exit(ModuleCharacterDeviceExit);
