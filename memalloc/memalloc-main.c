/* General headers */
#include <linux/kthread.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/skbuff.h>
#include <linux/freezer.h>
#include <linux/mutex.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/highmem.h>
#include <asm/pgtable.h>
#include <asm/tlbflush.h>
#include <linux/vmalloc.h>
#include <asm/pgalloc.h>

/* File IO-related headers */
#include <linux/fs.h>
#include <linux/bio.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/blkdev.h>

/* Sleep and timer headers */
#include <linux/hrtimer.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/sched/types.h>
#include <linux/pci.h>

#include "../common.h"

/* Simple licensing stuff */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("student");
MODULE_DESCRIPTION("Project 2, CSE 330 Spring 2024");
MODULE_VERSION("0.01");

/* Calls which start and stop the ioctl teardown */
bool memalloc_ioctl_init(void);
void memalloc_ioctl_teardown(void);

/* Project 2 Solution Variable/Struct Declarations */
#define MAX_PAGES           4096
#define MAX_ALLOCATIONS     100

/* Page table allocation helper functions defined in kmod_helper.c */
pud_t*  memalloc_pud_alloc(p4d_t* p4d, unsigned long vaddr);
pmd_t*  memalloc_pmd_alloc(pud_t* pud, unsigned long vaddr);
void    memalloc_pte_alloc(pmd_t* pmd, unsigned long vaddr);

#if defined(CONFIG_X86_64)
    #define PAGE_PERMS_RW		PAGE_SHARED
    #define PAGE_PERMS_R		PAGE_READONLY
#else
    #define PAGE_PERMS_RW		__pgprot(_PAGE_DEFAULT | PTE_USER | PTE_NG | PTE_PXN | PTE_UXN | PTE_WRITE)
    #define PAGE_PERMS_R		__pgprot(_PAGE_DEFAULT | PTE_USER | PTE_NG | PTE_PXN | PTE_UXN | PTE_RDONLY)
#endif

struct alloc_info           alloc_req;
struct free_info            free_req;

/* Init and Exit functions */
static int __init memalloc_module_init(void) {
    printk("Hello from the memalloc module!\n");
    return 0;
}

static void __exit memalloc_module_exit(void) {
    /* Teardown IOCTL */
    printk("Goodbye from the memalloc module!\n");
}

/* IOCTL handler for vmod. */
static long memalloc_ioctl(struct file *f, unsigned int cmd, unsigned long arg) {
    switch (cmd)
    {
    case ALLOCATE:    	
        /* allocate a set of pages */
        printk("IOCTL: alloc(%lx, %d, %d)\n", alloc_req.vaddr, alloc_req.num_pages, alloc_req.write);
        break;
    case FREE:    	
        /* free allocated pages */
    	printk("IOCTL: free(%lx)\n", free_req.vaddr);
    	break;    	
    default:
        printk("Error: incorrect IOCTL command.\n");
        return -1;
    }
    return 0;
}

/* Required file ops. */
static struct file_operations fops = 
{
    .owner          = THIS_MODULE,
    .unlocked_ioctl = memalloc_ioctl,
};

/* Initialize the module for IOCTL commands */
bool memalloc_ioctl_init(void) {
    return false;
}

void memalloc_ioctl_teardown(void) {
    /* Destroy the classes too (IOCTL-specific). */
}

module_init(memalloc_module_init);
module_exit(memalloc_module_exit);