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


/* meta information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kishwar Kumar");
MODULE_DESCRIPTION("This is a basic Linux kernel pseudo character device driver.");

/* lets create a memory buffer on which we will do operations */
#define PSEUDO_DEVICE_MEMORY_BUFFER 512
char pseudo_device_buffer[PSEUDO_DEVICE_MEMORY_BUFFER];

/* lets store device number */
dev_t device_number;

/*cdev variable*/
struct cdev pcdev;

/*-------------------------------------------------------------------*/
/*define global functions*/
loff_t _lseek(struct file *pfile, loff_t off, int whence)
{
  printk("executing %s\n", __func__);
  return 0;
}

ssize_t _read(struct file *pfile, char __user *pbuff, size_t count, loff_t *poff)
{
  printk("executing %s, requested %zu bytes\n", __func__, count);
  return 0;
}

ssize_t _write(struct file *pfile, const char __user *pbuff, size_t count, loff_t *poff)
{
  printk("executing %s, requested %zu bytes\n", __func__, count);
  return 0;
}

int _open(struct inode *node, struct file *pfile)
{
  printk("executing %s\n", __func__);
  return 0;
}

int _release(struct inode *pnode, struct file *pfile)
{
  printk("executing %s\n", __func__);
  return 0;
}

/*file operations of the driver*/
struct file_operations pcfops =
{
  .open    = _open,
  .write   = _write,
  .read    = _read,
  .llseek  = _lseek,
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
  printk("executing %s\n", __func__);

  /*1. dynamically allocate a device number (creates device number)*/
  alloc_chrdev_region(&device_number, 0 /*first minor*/, 1 /*counts*/, "pdevice");

  printk("%s device number <major>:<minor> = %d:%d\n", __func__,
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

  /*2. initialize the cdev structure with fops*/
  cdev_init(&pcdev, &pcfops);

  /*3. register a device (cdev structure) with VFS*/
  pcdev.owner = THIS_MODULE;
  cdev_add(&pcdev, device_number, 1);
  
  /*4. create device class under /sys/class/ */
  pdclass = class_create(THIS_MODULE, "pdevclass");

  /*5. populate the sysfs with the device information*/
  pdevice = device_create(pdclass, NULL, device_number, NULL, "pdev");

  printk("%s device created successfully..\n", __func__);
  return 0;
}

/*
 * @brief this function is called, when the module is removed from the kernel
 */
static void __exit ModuleCharacterDeviceExit(void)
{
  printk("executing %s\n", __func__);
  
  /*cleanup task*/
  device_destroy(pdclass, device_number);
  class_destroy(pdclass);
  cdev_del(&pcdev);
  unregister_chrdev_region(device_number, 1);

  printk("%s device cleaned up successfully..\n", __func__);
}

module_init(ModuleCharacterDeviceInit);
module_exit(ModuleCharacterDeviceExit);
