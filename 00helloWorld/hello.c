#include <linux/module.h>
#include <linux/init.h>

/* meta information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kishwar Kumar");
MODULE_DESCRIPTION("HelloWorld linux kernel module (LKM)");

/*
 * @brief this function is called, when the module is loaded into the kernel
 */
static int __init ModuleHelloWorldInit(void)
{
	printk("Hello, Kernel module (LKM)");
	return 0;
}

/*
 * @brief this function is called, when the module is removed from the kernel
 */
static void __exit ModuleHelloWorldExit(void)
{
	printk("Goodbye, Kernel module (LKM)");
}

module_init(ModuleHelloWorldInit);
module_exit(ModuleHelloWorldExit);

