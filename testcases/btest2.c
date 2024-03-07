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
 * Testing "Hello" and "Goodbye" message sending between parent and child process.
 */

bool test_share_page(int pid){
    /* Allocate a page */
    struct alloc_info* allocCall;    
    allocCall = malloc(sizeof(struct alloc_info));
    allocCall->vaddr                = 0x10000000;
    allocCall->num_pages            = 1;
    allocCall->write                = true;
    if (!ioctl(devfd, ALLOCATE, allocCall) < 0) {
        return false;
    }

    /* Testing read permissions on allocated page */
    char* vaddr_ptr = (char*) allocCall->vaddr;
    strcpy (vaddr_ptr, "Hello World!\0");

    /* Share the allocated page */
    struct sharepage_info* sharePageCall;    
    sharePageCall = malloc(sizeof(struct sharepage_info));
    sharePageCall->vaddr                = 0x10000000;
    sharePageCall->client_pid           = pid;
    sharePageCall->write                = true;
    if (!ioctl(devfd, SHAREPAGE, sharePageCall) < 0) {
        perror("Failed to share the page. \n");
        return false;
    }

    return true;
}

bool test_access_shared_page(void){
    struct accesspage_info* accessPageCall;
    accessPageCall          = malloc(sizeof(struct free_info));
    accessPageCall->vaddr   = 0x20000000;
    if (ioctl(devfd, ACCESSPAGE, accessPageCall) < 0) {
        perror("Failed to execute accesspage ioctl correctly.\n");
        return false;
    }

    /* Testing read permissions on allocated page */
    char* vaddr_ptr = (char*) 0x20000000;
    printf("Message from parent process: %s\n", vaddr_ptr); 

    /* Send a message back */
    strcpy (vaddr_ptr, "Goodbye World!\0");

    return true;
}

int main(void)
{
    printf("Executing: BTEST2\n");

    if (!open_device_driver()) return -1;

    register_segfault_handler();

    /* Fork a new process which will try to access the shared page */
    int pid = fork();

    if (pid != 0) {
        /* Parent process */
        if(!test_share_page(pid)) {
            printf("Page Allocation and Sharing Failed!\n");
            return -1;
        }
    } else {
        /* Child process */
        sleep(2);
        if(!test_access_shared_page()){
            printf("Failed: Page access was not possible.\n");
            return -1;
        }
    }

    // Make the parent process wait
    if (pid != 0) {
        wait(NULL);
        printf("Message from child process: %s\n", (char*) 0x10000000); 

        printf("Passed: BTEST2\n");
        close(devfd);
    }

    return 0;
}


