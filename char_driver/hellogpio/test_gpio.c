#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "myioctl.h"

// sudo mknod /dev/mygpio c 10 99

int main(int argc, char **argv)
{
    int fd;
    int err;

    fd = open("/dev/mygpio", 0);
    if (fd < 0) {
        perror("open failed\n");
        return -1;
    }

    err = ioctl(fd, IOCTL_SET_PIN_MODE, PIN_MODE_OUTPUT);
    if (err < 0) {
        perror("ioctl IOCTL_SET_PIN_MODE failed\n");
        close(fd);
        return -1;
    }

    ioctl(fd, IOCTL_SET_PIN_DIGITAL, LOW);
    if (err < 0) {
        perror("ioctl IOCTL_SET_PIN_DIGITAL LOW failed\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
