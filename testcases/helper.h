#ifndef __HELPER_H__
#define __HELPER_H__

#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define RCX_INDEX 13

char* devname = "/dev/memalloc";
int   devfd = -1;

unsigned long vaddr; 
unsigned long paddr;
unsigned long num_pages;

bool open_device_driver(void) {
    devfd = open(devname, O_RDWR);
    if (devfd < 0) {
        printf("Error: could not open device (%s)\n", devname);
        return false;
    }
    return true;
}

void segfault_handler(int nSignum, siginfo_t* si, void* vcontext) {
    printf("Caught: Segmentation Fault.\n");

    ucontext_t* context = (ucontext_t*)vcontext;
    
#if defined(__x86_64)
    printf("Address: %p\n", (void*) context->uc_mcontext.gregs[RCX_INDEX]);
#elif defined(__aarch64__)
    printf("Address: %p\n", (void*) context->uc_mcontext.regs[0]);
#else
    printf("Unsupported Architecture\n");
#endif
    
    exit(-1);
}

void register_segfault_handler(void) {
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_flags = SA_SIGINFO;
  action.sa_sigaction = segfault_handler;
  sigaction(SIGSEGV, &action, NULL);
}

#endif
