
#include <linux/init.h>
#include <linux/module.h>
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
#include "cmd.h"

MODULE_LICENSE("Dual BSD/GPL");

// 设备号
#define GPIO_MAJOR 10
#define GPIO_MINOR 99

#define GPIO_NAME "hello"

static struct cdev g_dev;
static struct class *g_class;

// 函数作用域只在当前文件有效
static int open (struct inode *inode, struct file *filep)
{
    printk(KERN_ALERT"open success\n");
    return 0;
}
static int release (struct inode *inode, struct file *filep)
{
    printk(KERN_ALERT"release success\n");
    return 0;
}

#define GPIO_PIN 21
#define GPIO_PIN_21_CONFIGURE_ADDR  0x3f200008
#define GPIO_PIN_21_OUTPUT_SET_ADDR 0x3f20001c
#define GPIO_PIN_21_OUTPUT_CLEAR_ADDR 0x3f200028

// 配置引脚
static void gpio_set_pin_mode(void)
{
    void *addr;
    // 禁止编译器优化
    volatile uint32_t value;
    // 1. 映射, ioremap
    addr = ioremap(GPIO_PIN_21_CONFIGURE_ADDR, 4);
    // 2. 读取, ioread32
    value = ioread32(addr);
    // 3. 改
    // 3. 1清零
    value = value & ~(0x7 << 3);
    // 3.2 置位
    value = value | (0x1 << 3);
    // 4. 写入, iowrite32
    iowrite32(value, addr);
}

// 设置低电平
static void gpio_output_set(void)
{
    void *addr;
    // 禁止编译器优化
    volatile uint32_t value;
    // 1. 映射, ioremap
    addr = ioremap(GPIO_PIN_21_OUTPUT_SET_ADDR, 4);
    // 2. 读取, ioread32
    value = ioread32(addr);
    // 3. 改
    // 3. 1清零
    value = value & ~(0x1 << 21);
    // 3.2 置位
    value = value | (0x1 << 21);
    // 4. 写入, iowrite32
    iowrite32(value, addr);
}

// 设置高电平
static void gpio_output_clear(void)
{
    void *addr;
    // 禁止编译器优化
    volatile uint32_t value;
    // 1. 映射, ioremap
    addr = ioremap(GPIO_PIN_21_OUTPUT_CLEAR_ADDR, 4);
    // 2. 读取, ioread32
    value = ioread32(addr);
    // 3. 改
    // 3. 1清零
    value = value & ~(0x1 << 21);
    // 3.2 置位
    value = value | (0x1 << 21);
    // 4. 写入, iowrite32
    iowrite32(value, addr);
}

static long unlocked_ioctl (struct file *filep, unsigned int cmd, unsigned long arg)
{
    uint32_t myarg = (uint32_t)arg;
    switch (cmd) {
        case CMD_PIN_MODE:
            printk(KERN_ALERT"CMD_PIN_MODE\n");
            gpio_set_pin_mode();
            break;
        case CMD_PIN_LEVEL:
            printk(KERN_ALERT"CMD_PIN_LEVEL, arg %d\n", myarg);
            if (myarg == PIN_LEVEL_LOW) {
                gpio_output_set();
            } else {
                gpio_output_clear();
            }

            break;
        default:
            return -EFAULT;
    }
    return 0;
}


struct file_operations myops = {
    .owner = THIS_MODULE,
    .open = open,
    .release = release,
    .unlocked_ioctl = unlocked_ioctl,
};

// 跟hello_init的顺序相反
static void cleanup(int device_created, int cdev_created)
{
    dev_t devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);

    if (device_created) {
        // 删除设备节点
        device_destroy(g_class, devno);
    }

    if (cdev_created) {
        // 向内核注销
        cdev_del(&g_dev);
    }

    // 删除设备号
    unregister_chrdev_region(devno, 1);
}

static int hello_init(void)
{
    int device_created = 0;
    int cdev_created = 0;
    int ret;
    int devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);
    printk(KERN_ALERT"Hello, world\n");

    // 1)注册设备号
    ret = register_chrdev_region(0, 1, GPIO_NAME);
    if (ret < 0) {
        printk(KERN_ALERT"register_chrdev_region failed\n");
        return -1;
    }

    // 2)向sysfs注册设备节点
    if (NULL == device_create(g_class, NULL, GPIO_MAJOR, NULL, GPIO_NAME)) {
        printk(KERN_ALERT"device_created failed\n");
        goto cleanup;
    }
    device_created = 1;

    // 3)向内核注册

    cdev_init(&g_dev, &myops);
    g_dev.owner = THIS_MODULE;
    g_dev.ops = &myops;

    ret = cdev_add(&g_dev, devno, 1);
    if (ret < 0) {
        printk(KERN_ALERT"cdev_add failed\n");
        goto cleanup;
    }
    cdev_created = 1;

    return 0;

cleanup:

    // 清理已经申请的资源
    cleanup(device_created, cdev_created);
    return -1;
}

static void hello_exit(void)
{
    printk(KERN_ALERT"Goodbye, cruel world\n");
    cleanup(1, 1);
}

module_init(hello_init);
module_exit(hello_exit);

