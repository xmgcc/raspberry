#include <linux/cdev.h> // cdev_add cdev_init
#include <linux/kdev_t.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h> // struct file_operations

#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/unistd.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>

//#include "bcm2835.h"


MODULE_LICENSE("Dual BSD/GPL");

int hello_open(struct inode *pnode, struct file *filp)
{
    return -1;
}

int hello_release(struct inode *pnode, struct file *filp)
{
    return -1;
}

struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = hello_open,
    .release = hello_release,
};

static int hello_init(void)
{
    printk(KERN_ALERT"Hello, world\n");
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT"Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);

