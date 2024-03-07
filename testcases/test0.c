#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>

#include "../common.h"
#include "helper.h"

/* 
 * Description:
 * This test simply checks if your IOCTL calls are correctly being
 * sent to the kernel module through the device interface.
 */

bool test_allocate_ioctl(void){
    struct alloc_info* allocCall;    
    allocCall                       = malloc(sizeof(struct alloc_info));
    allocCall->vaddr                = vaddr;
    allocCall->num_pages            = num_pages;
    allocCall->write                = true;
    if (ioctl(devfd, ALLOCATE, allocCall) < 0) {
        return false;
    }
    return true;
}

bool test_free_ioctl(void){
    struct free_info* freeCall;
    freeCall = malloc(sizeof(struct free_info));
    freeCall->vaddr = vaddr;
    if (ioctl(devfd, FREE, freeCall) < 0) {
        return false;
    }
    return true;
}

int main(void)
{
    printf("Executing: TEST0\n");
    if (!open_device_driver()) {
        printf("[x] Device is not registered\n");
        return -1;
    }
    printf("[*] Device opened successfully\n");

    vaddr       = 0x10000000;   // Set the desired virtual address
    num_pages   = 1;            // Set the number of pages to allocate
    paddr       = 0x0;
    
    test_allocate_ioctl();
    test_free_ioctl();
    
    printf("Test complete, check the output in dmesg log.\n");
    close(devfd);
    return 0;
}
