#ifndef MYIOCTL_H
#define MYIOCTL_H

#include <linux/ioctl.h>

#define GPIO_MAJOR 10

// 参数1, 驱动的主设备号
// 参数2, 第0个ioctl命令
// 参数3, 参数arg的类型
//long ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#define IOCTL_SET_PIN_MODE _IOW(GPIO_MAJOR, 0, int)

#define IOCTL_SET_PIN_DIGITAL _IOW(GPIO_MAJOR, 1, int)

// datasheet
// 000 INPUT
// 001 OUTPUT
#define PIN_MODE_INPUT 0
#define PIN_MODE_OUTPUT 1

#define LOW 0
#define HIGH 1

#endif // MYIOCTL_H
