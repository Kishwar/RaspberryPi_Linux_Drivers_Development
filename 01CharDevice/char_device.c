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

#define MODULE_NAME "SINGLE_CHAR_DEVICE"

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
  loff_t cursor;
  pr_info("%s: executing %s\n", MODULE_NAME, __func__);

  switch(whence)
  {
    case SEEK_SET:
      if((off > PSEUDO_DEVICE_MEMORY_BUFFER) || (off < 0)) return -EINVAL;
      pfile->f_pos = off;
      break;
    case SEEK_CUR:
      cursor = pfile->f_pos + off;
      if((cursor > PSEUDO_DEVICE_MEMORY_BUFFER) || (cursor < 0)) return -EINVAL;
      pfile->f_pos += off;
      break;
    case SEEK_END:
      cursor = PSEUDO_DEVICE_MEMORY_BUFFER + off;
      if((cursor > PSEUDO_DEVICE_MEMORY_BUFFER) || (cursor < 0)) return -EINVAL;
      pfile->f_pos = PSEUDO_DEVICE_MEMORY_BUFFER + off;
      break;
    default:
      return -EINVAL;
  }

  return pfile->f_pos;
}

ssize_t _read(struct file *pfile, char __user *pbuff, size_t count, loff_t *poff)
{
  pr_info("%s: executing %s, requested %zu bytes\n", MODULE_NAME, __func__, count);

  if(pbuff == NULL || poff == NULL)
  {
    pr_err("%s: %s invalid parameters.\n", MODULE_NAME, __func__);
    return -EINVAL;
  }

  if((*poff + count) > PSEUDO_DEVICE_MEMORY_BUFFER)
  {
    count = PSEUDO_DEVICE_MEMORY_BUFFER - *poff;
  }

  /*copy data to user (never ever believe on user buffer)*/
  if(copy_to_user(pbuff, pseudo_device_buffer + *poff, count))
  {
    pr_err("%s: %s copy_to_user failed.\n", MODULE_NAME, __func__);
    return -EFAULT;
  }

  /*update the current file position and return count*/
  *poff += count;
  return count;
}

ssize_t _write(struct file *pfile, const char __user *pbuff, size_t count, loff_t *poff)
{
  pr_info("%s: executing %s, requested %zu bytes\n", MODULE_NAME, __func__, count);

  if(pbuff == NULL || poff == NULL)
  {
    pr_err("%s: %s invalid parameters.\n", MODULE_NAME, __func__);
    return -EINVAL;
  }

  if((*poff + count) > PSEUDO_DEVICE_MEMORY_BUFFER)
  {
    count = PSEUDO_DEVICE_MEMORY_BUFFER - *poff;
  }

  if(!count)
  {
    pr_err("%s: %s buffer full. no space available.\n", MODULE_NAME, __func__);
    return -ENOMEM;
  }

  /*copy data to user (never ever believe on user buffer)*/
  if(copy_from_user(pseudo_device_buffer + *poff, pbuff, count))
  {
    pr_err("%s: %s copy_to_user failed.\n", MODULE_NAME, __func__);
    return -EFAULT;
  }

  /*update the current file position and return count*/
  *poff += count;
  return count;
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

  pr_info("%s: %s device cleaned up successfully..\n", MODULE_NAME, __func__);
}

module_init(ModuleCharacterDeviceInit);
module_exit(ModuleCharacterDeviceExit);
