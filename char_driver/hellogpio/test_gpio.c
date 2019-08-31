#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

// sudo mknod /dev/mygpio c 10 99

int main(int argc, char **argv)
{
    int fd;

    fd = open("/dev/mygpio", 0);
    if (fd < 0) {
        perror("open failed");
        return -1;
    }

    close(fd);
    return 0;
}
