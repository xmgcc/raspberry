#ifndef CMD_H
#define CMD_H
#include <linux/ioctl.h>

#define HELLO_MAGIC 'h'


#define PIN_MODE_INPUT 0
#define PIN_MODE_OUTPUT 1

// 配置GPIO
#define CMD_PIN_MODE    _IOW(HELLO_MAGIC, 0, int)


#define PIN_LEVEL_HIGH 1
#define PIN_LEVEL_LOW  0
// 配置GPIO的高低电平
#define CMD_PIN_LEVEL   _IOW(HELLO_MAGIC, 1, int)
#endif // CMD_H
