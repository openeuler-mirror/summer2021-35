#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
MODULE_LICENSE("GPL");
static int hello_init(void)
{
    printk(KERN_ALERT "Hello World enter/n");
    return 0;
}
static void hello_exit(void)
{
    printk(KERN_ALERT "Hello World exit/n");
}
module_init(hello_init);
module_exit(hello_exit);
