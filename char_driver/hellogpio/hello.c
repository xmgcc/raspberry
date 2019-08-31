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
#include "myioctl.h"

#define GPIO_MINOR 99

//#include "bcm2835.h"
static struct cdev *mydev;

static struct class *myclass = NULL;


MODULE_LICENSE("Dual BSD/GPL");

// static 修饰函数，函数不能被其他文件的函数调用
static int hello_open(struct inode *pnode, struct file *filp)
{
    printk("open success\n");
    return 0;
}

static int hello_release(struct inode *pnode, struct file *filp)
{
    printk("close success\n");
    return 0;
}

// 使用GPIO29
#define GPIO_PIN 29

// arm MMU后的基址 + 偏移
// 0x20000000 + 0x00200000
#define GPIO_BASE_ADDR   0x20200000
#define GPIO_SIZE 4096

static void gpio_function_select(int mode)
{
    volatile uint32_t *fsel_reg;
    uint32_t val = 0;

    // 映射
    fsel_reg = ioremap(GPIO_BASE_ADDR, 8);

    // 读取原始值
    val = ioread32(fsel_reg);
    printk("gpio_function_select val 0x%x\n", val);

    // clear
    val &= ~(0x7 << ((GPIO_PIN % 10) * 3));

    // set
    val |= mode << ((GPIO_PIN % 10) * 3);

    printk("gpio_function_select val 0x%x\n", val);

    // 写入新值
    iowrite32(val, fsel_reg);
}

static void gpio_pin_output_set(uint32_t pin)
{
    volatile uint32_t *fsel_reg;
    uint32_t val = 0;

    // 映射
    fsel_reg = ioremap(GPIO_BASE_ADDR, 0x1c);

    // 读取原始值
    val = ioread32(fsel_reg);

    printk("gpio_pin_output_set val 0x%x\n", val);

    // set
    val |= 1 << pin;

    printk("gpio_pin_output_set val 0x%x\n", val);

    // 写入新值
    iowrite32(val, fsel_reg);
}

static void gpio_pin_output_clear(uint32_t pin)
{
    volatile uint32_t *fsel_reg;
    uint32_t val = 0;

    // 映射
    fsel_reg = ioremap(GPIO_BASE_ADDR, 0x28);

    // 读取原始值
    val = ioread32(fsel_reg);

    printk("gpio_pin_output_clear val 0x%x\n", val);

    // set
    val |= 1 << pin;

    printk("gpio_pin_output_clear val 0x%x\n", val);

    // 写入新值
    iowrite32(val, fsel_reg);
}

/* int bcm2835_gpio_fsel(uint8_t pin, uint8_t mode) */
/* { */
/*     //初始化GPIOB功能选择寄存器的物理地址 */
/*     volatile uint32_t * bcm2835_gpio = (volatile uint32_t *)ioremap(BCM2835_GPIO_BASE, 16); */
/*     volatile uint32_t * bcm2835_gpio_fsel = bcm2835_gpio + BCM2835_GPFSEL0/4 + (pin/10); */
/*     uint8_t   shift = (pin % 10) * 3; */
/*     uint32_t  value = mode << shift; */
/*     *bcm2835_gpio_fsel = *bcm2835_gpio_fsel | value; */
/*   */
/*     printk("fsel address: 0x%lx : %x\n", bcm2835_gpio_fsel, *bcm2835_gpio_fsel); */
/*   */
/*     return 0; */
/* } */
/*   */
/* int bcm2835_gpio_set(uint8_t pin) */
/* { */
/*     //GPIO输出功能物理地址 */
/*     volatile uint32_t * bcm2835_gpio = (volatile uint32_t *)ioremap(BCM2835_GPIO_BASE, 16); */
/*     volatile uint32_t * bcm2835_gpio_set = bcm2835_gpio + BCM2835_GPSET0/4 + pin/32; */
/*     uint8_t   shift = pin % 32; */
/*     uint32_t  value = 1 << shift; */
/*     *bcm2835_gpio_set = *bcm2835_gpio_set | value; */
/*   */
/*     printk("set address:  0x%lx : %x\n", bcm2835_gpio_set, *bcm2835_gpio_set); */
/*   */
/*     return 0; */
/* } */
/*   */
/* int bcm2835_gpio_clr(uint8_t pin) */
/* { */
/*    //GPIO清除功能物理地址 */
/*     volatile uint32_t * bcm2835_gpio = (volatile uint32_t *)ioremap(BCM2835_GPIO_BASE, 16); */
/*     volatile uint32_t * bcm2835_gpio_clr = bcm2835_gpio + BCM2835_GPCLR0/4 + pin/32; */
/*     uint8_t   shift = pin % 32; */
/*     uint32_t  value = 1 << shift; */
/*     *bcm2835_gpio_clr = *bcm2835_gpio_clr | value; */
/*      */
/*     printk("clr address:  0x%lx : %x\n", bcm2835_gpio_clr, *bcm2835_gpio_clr); */
/*   */
/*     return 0; */
/* } */

static long hello_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    uint32_t myarg = (uint32_t)arg;

    switch (cmd)
    {
        case IOCTL_SET_PIN_MODE:
            printk("IOCTL_SET_PIN_MODE, arg %d\n", myarg);
            gpio_function_select(myarg);
            break;
        case IOCTL_SET_PIN_DIGITAL:
            printk("IOCTL_SET_PIN_DIGITAL, art %d\n", myarg);
            if (myarg == LOW) {
                gpio_pin_output_set(GPIO_PIN);
            } else {
                gpio_pin_output_clear(GPIO_PIN);
            }
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

// linux/fs.h
struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = hello_open,
    .unlocked_ioctl = hello_unlocked_ioctl,
    .release = hello_release,
};

static void cleanup(int device_created)
{
    dev_t devno;
    printk(KERN_ALERT"Goodbye, cruel world\n");

    /* release_mem_region(GPIO_BASE_ADDR, GPIO_SIZE); */

    devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);
    if (mydev) {
        cdev_del(mydev);
        mydev = NULL;
    }

    if (device_created) {
        device_destroy(myclass, GPIO_MAJOR);
    }

    unregister_chrdev_region(devno, 1);
}

#define GPIO_NAME "mygpio"
static int hello_init(void)
{
    // 注册主次设备号
    int err;
    dev_t dev = 0;
    int device_created = 0;
    int devno;
    printk(KERN_ALERT"Hello, world, %s\n", __TIME__);

    devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);
    err = register_chrdev_region(dev, 1, GPIO_NAME);
    if (err < 0) {
        printk("register_chrdev_region failed\n");
        return -1;
    }

    /* if (request_mem_region(GPIO_BASE_ADDR, GPIO_SIZE, GPIO_NAME) == NULL) { */
    /*     printk("request_mem_region failed\n"); */
    /*     return -ENODEV; */
    /* } */


    // ls /dev/mygpio
    if (device_create(myclass, NULL, GPIO_MAJOR, NULL, "mygpio") == NULL) {
        printk("device_created failed\n");
        goto cleanup;
    }

    device_created = 1;

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
    cleanup(device_created);

    // return -1模块就不会被内核加载，因此需要把申请的资源先释放掉
    return -1;
}

static void hello_exit(void)
{
    cleanup(1);
}

module_init(hello_init);
module_exit(hello_exit);

