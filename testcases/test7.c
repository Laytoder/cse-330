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
 * This test allocates 4096 pages with R+W permissions from two different processes.
 */

bool test_allocate(void) {
    struct alloc_info* allocCall;    
    int allocations = 101;
    allocCall                       = malloc(sizeof(struct alloc_info));
    allocCall->num_pages            = 1;
    allocCall->write                = false;
    int alloc = 0;

    while (allocations > 0) {
        alloc++;
        allocCall->vaddr            = 0x10000000 + (allocations*4096);
        if (allocations != 1) {
            if (ioctl(devfd, ALLOCATE, allocCall) != 0) {
                return false;
            }
            printf("Allocation #%d passed\n", (alloc));
            
            int* vaddr_ptr = (int*) (allocCall->vaddr);
            printf(" READing (%lx)\n", (unsigned long) vaddr_ptr);
            assert(*vaddr_ptr == 0);
        }
        else {
            ioctl(devfd, ALLOCATE, allocCall);
            if (errno  != 3) {
                return false;
            }
            printf("Allocation #%d failed correctly\n", (alloc));
        }
        allocations--;
    }

    return true;
}

int main(void)
{
    printf("Executing: TEST7\n");

    if (!open_device_driver()) return -1;

    if(!test_allocate()) {
    	printf("Failed: TEST7\n");
        return -1;
    }

    printf("Passed: TEST7\n");
    close(devfd);
    return 0;
}