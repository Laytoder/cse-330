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
 * This test allocates 199 pages with R+W permissions and 1 page with read permission. 
 * Checks if the read-only page is correctly set.
 */

bool test_allocate(void) {
    struct alloc_info* allocCall;    
    allocCall                       = malloc(sizeof(struct alloc_info));
    allocCall->vaddr                = 0x10000000;
    allocCall->num_pages            = 199;
    allocCall->write                = true;
    if (ioctl(devfd, ALLOCATE, allocCall) < 0) {
        return false;
    }

    /* Testing read permissions on allocated page */
    int* vaddr_ptr;
    int  pages = 0;
    while (pages < allocCall->num_pages) {
        vaddr_ptr = (int*) (allocCall->vaddr + (4096*pages));
        printf(" READing (%lx)\n", (unsigned long) vaddr_ptr);
        assert(*vaddr_ptr == 0);
        pages++;
    }
    printf("Passed: READ\n");

    /* Testing write permissions on allocated page */
    pages = 0;
    while (pages < allocCall->num_pages) {
        vaddr_ptr = (int*) (allocCall->vaddr + (4096*pages));
        printf(" WRITEing (%lx)\n", (unsigned long) vaddr_ptr);
        *vaddr_ptr = pages;
        assert(*vaddr_ptr == pages);
        pages++;
    }
    printf("Passed: WRITE\n");

    allocCall->vaddr                = 0x10000000 + (199*4096);
    allocCall->num_pages            = 1;
    allocCall->write                = false;
    if (!ioctl(devfd, ALLOCATE, allocCall) < 0) {
        return false;
    }    

    /* Testing read permissions on allocated page */
    pages = 0;
    while (pages < allocCall->num_pages) {
        vaddr_ptr = (int*) (allocCall->vaddr + (4096*pages));
        printf(" READing (%lx)\n", (unsigned long) vaddr_ptr);
        assert(*vaddr_ptr == 0);
        pages++;
    }
    printf("Passed: READ\n");

    /* Testing write permissions on allocated page */
    pages = 0;
    while (pages < allocCall->num_pages) {
        vaddr_ptr = (int*) (allocCall->vaddr + (4096*pages));
        printf(" WRITEing (%lx)\n", (unsigned long) vaddr_ptr);
        *vaddr_ptr = pages;
        assert(*vaddr_ptr == pages);
        pages++;
    }
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
    printf("Executing: TEST5\n");

    if (!open_device_driver()) return -1;
    
    register_segfault_handler();

    if(!test_allocate()) {
    	printf("Allocate Failed!\n");
        return -1;
    }
    
    if(!test_free()){
    	printf("Free Failed!\n");
        return -1;
    }

    printf("Passed: TEST5\n");
    close(devfd);
    return 0;
}
