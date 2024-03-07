#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>

/* 
 * Description:
 * This is a usermode example of how ioctl calls can be made.
 */

char* devname = "/dev/virtdev";
int   devfd = -1;

struct ioctl_data {
	int number;
};
#define EXAMPLE_IOCTL		_IOW('a', 'a', struct ioclt_data *)

bool open_device_driver(void) {
    devfd = open(devname, O_RDWR);
    if (devfd < 0) {
        printf("Error: could not open device (%s)\n", devname);
        return false;
    }
    return true;
}

int main(void)
{
    printf("Sample: virtdev\n");
    if (!open_device_driver()) {
        return -1;
    }
    struct ioctl_data* test = (struct ioctl_data*) malloc(sizeof(struct ioctl_data));
    test->number = 0xdeadbeef;
    if (!ioctl(devfd, EXAMPLE_IOCTL, test) < 0) {
        return false;
    }    
    close(devfd);
    return 0;
}