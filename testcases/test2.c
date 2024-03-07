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

#include "../common.h"
#include "helper.h"

/* 
 * Description:
 * This test allocates a page with R+W permissions and checks that
 * the page can be allocated, accessed, and freed correctly.
 */

bool test_allocate(void){
    struct alloc_info* allocCall;    
    allocCall = malloc(sizeof(struct alloc_info));
    allocCall->vaddr                = 0x10000000;
    allocCall->num_pages            = 1;
    allocCall->write                = true;
    if (ioctl(devfd, ALLOCATE, allocCall) < 0) {
        return false;
    }

    /* Testing read permissions on allocated page */
    int* vaddr_ptr = (int*) allocCall->vaddr;
    assert(*vaddr_ptr == 0);
    printf("Passed: READ\n");

    /* Testing write permissions on allocated page */
    *vaddr_ptr = 1;
    assert(*vaddr_ptr == 1);
    printf("Passed: WRITE\n");

    return true;
}

bool test_free(void){
    struct free_info* freeCall;
    freeCall = malloc(sizeof(struct free_info));
    freeCall->vaddr = 0x10000000;   
    if (ioctl(devfd, FREE, freeCall) < 0) {
        perror("Failed to free pages");
        return false;
    }
    return true;
}

int main(void)
{
    printf("Executing: TEST2\n");

    if (!open_device_driver()) return -1;
    
    if(!test_allocate()) {
    	printf("Allocate Failed!\n");
        return -1;
    }
    
    if(!test_free()){
    	printf("Free Failed!\n");
        return -1;
    }

    printf("Passed: TEST2\n");
    close(devfd);
    return 0;
}


