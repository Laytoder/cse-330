#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <errno.h>

#include "../common.h"
#include "helper.h"

/* 
 * Description:
 * This test allocates 4097 pages with R+W permissions and checks that
 * allocation fails as expected.
 */

bool test_allocate(void) {
    struct alloc_info* allocCall;    
    allocCall                       = malloc(sizeof(struct alloc_info));
    allocCall->vaddr                = 0x10000000;
    allocCall->num_pages            = 4097;
    allocCall->write                = false;
    long ret = ioctl(devfd, ALLOCATE, allocCall);
    if (errno != 2) {
        printf("errno = %d\n", errno);
        return false;
    }
    
    printf("Allocation failed correctly\n");
    return true;
}

int main(void)
{
    printf("Executing: TEST6\n");

    if (!open_device_driver()) return -1;
    
    if(!test_allocate()) {
    	printf("Failed: TEST6\n");
        return -1;
    }

    printf("Passed: TEST6\n");
    close(devfd);
    return 0;
}
