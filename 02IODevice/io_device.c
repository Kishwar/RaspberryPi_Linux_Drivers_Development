/************************************************************
 *  io_driver.c - Simple Linux Kernel IO LED Driver
 *
 *  Description:
 *      This is a basic Linux kernel io led device driver.
 *      The driver implements turning on / off features
 *      It can be used as a template for developing more complex
 *      character io drivers.
 *
 *  Functionality:
 *      - Registers a character device (io) with the kernel.
 *      - Implements write with turn on / off / toggle feature
 *
 *  Usage:
 *      - To compile: `make`
 *      - To load: `sudo insmod io_driver.ko`
 *      - To remove: `sudo rmmod io_driver`
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
#include <linux/gpio.h>

/* meta information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kishwar Kumar");
MODULE_DESCRIPTION("This is a basic Linux kernel character device driver for io operation.");

#define MODULE_NAME "SINGLE_CHAR_IO_DEVICE"

/* lets store device number */
dev_t device_number;

/*cdev variable*/
struct cdev pcdev;

ssize_t _write(struct file *pfile, const char __user *pbuff, size_t count, loff_t *poff)
{
  char value;
  int to_copy, not_copied, delta;

  pr_info("%s: executing %s, requested %zu bytes\n", MODULE_NAME, __func__, count);

  if(pbuff == NULL || poff == NULL)
  {
    pr_err("%s: %s invalid parameters.\n", MODULE_NAME, __func__);
    return -EINVAL;
  }

  /*get size of data to copy*/
  to_copy = min(count, sizeof(value));

  /*copy user data*/
  not_copied = copy_from_user(&value, pbuff, to_copy);

  /*setting the LED*/
  switch(value)
  {
    case '0':
      gpio_set_value(4, 0);
      break;
    case '1':
      gpio_set_value(4, 1);
      break;
    default:
      pr_err("%s: %s invalid value.\n", MODULE_NAME, __func__);
      break;
  } 

  /*calculate delta*/
  delta = to_copy - not_copied;

  return delta;
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
  .write   = _write,
  .read    = NULL,
  .release = _release,
  .owner   = THIS_MODULE
};

struct class *pdclass;
struct device *pdevice;

/*-------------------------------------------------------------------*/
/*define static functions*/
/*
 * @brief this function is called, when the module is loaded into the kernel
 */
static int __init ModuleCharacterDeviceInit(void)
{
  pr_info("%s: executing %s\n", MODULE_NAME, __func__);

  /*1. dynamically allocate a device number (creates device number)*/
  if(alloc_chrdev_region(&device_number, 0 /*first minor*/, 1 /*counts*/, "iodevice") < 0)
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
  pdclass = class_create(THIS_MODULE, "iodevclass");
  if (IS_ERR(pdclass))
  {
    unregister_chrdev_region(device_number, 1);
    pr_err("%s: %s Failed to register device class\n", MODULE_NAME, __func__);
    return PTR_ERR(pdclass);
  }

  /*3. Create the device file in /dev and set the permissions to 0666 */
  pdevice = device_create(pdclass, NULL, device_number, NULL, "pio");
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

  /*6. Init GPIO4 - Connected with LED*/
  if(gpio_request(4, "rpi-gpio-4"))
  {
    device_destroy(pdclass, device_number);
    class_destroy(pdclass);
    unregister_chrdev_region(device_number, 1);
    pr_err("%s: %s Failed to allocate GPIO 4\n", MODULE_NAME, __func__);
    return -1;
  }

  /*7. Set the direction of the PIN*/
  if(gpio_direction_output(4, 0))
  {
    device_destroy(pdclass, device_number);
    class_destroy(pdclass);
    unregister_chrdev_region(device_number, 1);
    gpio_free(4);
    pr_err("%s: %s Can not set GPIO 4 to out\n", MODULE_NAME, __func__);
    return -1;
  }

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
  device_destroy(pdclass, device_number);
  class_destroy(pdclass);
  cdev_del(&pcdev);
  unregister_chrdev_region(device_number, 1);
  gpio_set_value(4, 0);
  gpio_free(4);
  pr_info("%s: %s device cleaned up successfully..\n", MODULE_NAME, __func__);
}

module_init(ModuleCharacterDeviceInit);
module_exit(ModuleCharacterDeviceExit);
