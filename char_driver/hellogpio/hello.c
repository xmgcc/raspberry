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
struct cdev *mydev;


MODULE_LICENSE("Dual BSD/GPL");

int hello_open(struct inode *pnode, struct file *filp)
{
    printk("open success\n");
    return 0;
}

int hello_release(struct inode *pnode, struct file *filp)
{
    printk("close success\n");
}

struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = hello_open,
    .release = hello_release,
};

#define GPIO_MAJOR 10
#define GPIO_MINOR 99

static int hello_init(void)
{
    // 注册主次设备号
    int err;
    dev_t dev = 0;
    int devno;
    printk(KERN_ALERT"Hello, world\n");

    devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);
    err = register_chrdev_region(dev, 1, "mygpio");
    if (err < 0) {
        printk("register_chrdev_region failed\n");
        return -1;
    }

    // 创建struct cdev结构体，初始化
    mydev = cdev_alloc();
    mydev->owner = THIS_MODULE;
    mydev->ops = &my_fops;

    //向内核注册
    err = cdev_add(mydev, devno, 1);
    if (err < 0) {
        printk("cdev_add failed\n");
        goto cleanup;
    }

    return 0;

cleanup:
    if (mydev) {
        cdev_del(mydev);
        mydev = NULL;
    }

    unregister_chrdev_region(devno, 1);

    // return -1模块就不会被内核加载，因此需要把申请的资源先释放掉
    return -1;
}

static void hello_exit(void)
{
    dev_t devno;
    printk(KERN_ALERT"Goodbye, cruel world\n");

    devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);
    if (mydev) {
        cdev_del(mydev);
        mydev = NULL;
    }

    unregister_chrdev_region(devno, 1);
}

module_init(hello_init);
module_exit(hello_exit);

