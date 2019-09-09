#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "cmd.h"

// sudo mknod /dev/hello c 10 99

/* ./a.out 1 灭 */
/* ./a.out 0 亮 */
int main(int argc, char **argv)
{
    int fd;
    int err;
    int level = PIN_LEVEL_LOW;

    if (argc >= 2) {
        level = atoi(argv[1]);
    }

    fd = open("/dev/hello", 0);
    if (fd < 0) {
        perror("open failed\n");
        return -1;
    }

    err = ioctl(fd, CMD_PIN_MODE, PIN_MODE_OUTPUT);
    if (err < 0) {
        perror("ioctl failed");
    }
    err = ioctl(fd, CMD_PIN_LEVEL, level);
    if (err < 0) {
        perror("ioctl failed");
    }

    close(fd);
    return 0;
}
