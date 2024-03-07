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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adil Ahmad");
MODULE_DESCRIPTION("Page Walk Example");
MODULE_VERSION("0.01");

/* Init and Exit functions */
static int __init sample_init(void) {
    printk("[*] Sample page walk for 0xdead\n");

    unsigned long vaddr = 0xdead;

    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    /* Return pointer of the PGD. mm is the mm_struct of the process, address is the logical 
       address in the virtual memory space*/
    pgd = pgd_offset(current->mm, vaddr);
    if (pgd_none(*pgd)) {
        printk("Error: pgd should always be mapped (something is really wrong!).\n");
        goto out;
    }
    printk("PGD is allocated. \n");

    /* Return pointer to the P4D. pgd is the pointer of PGD, address is the logical address in 
       the virtual memory space.*/
    p4d = p4d_offset(pgd, vaddr);
    if (p4d_none(*p4d) || p4d_bad(*p4d)) {
        printk("No P4D allocated; page must be unmapped.");
        goto out;
    }
    printk("P4D is allocated. \n");

    /* Return pointer to the PUD. p4d is the pointer of P4D, address is the logical address in 
       the virtual memory space.*/
    pud = pud_offset(p4d, vaddr);
    if (pud_none(*pud)) {
        printk("No PUD allocated; page must be unmapped.");
        goto out;
    }
    printk("PUD is allocated. \n");


    /* Return pointer to the PMD. pud is the pointer of PUD, address is the logical address in 
       the virtual memory space.*/
    pmd = pmd_offset(pud, vaddr);
    if (pmd_none(*pmd)) {
        printk("No PMD allocated; page must be unmapped.");
        goto out;
    }
    printk("PMD is allocated. \n");

    /* Return pointer to the PTE. pmd is the pointer of PMD, address is the logical address in 
       the virtual memory space*/
    pte = pte_offset_kernel(pmd, vaddr);
    if (pte_none(*pte)) {
        printk("No PTE allocated; page must be unmapped.");
        goto out;
    }
    printk("PTE is allocated. \n");

    if (pte_present(*pte)) {
    	printk("Page is mapped.");
        goto out;
    }
    printk("Page is not mapped. \n");    // Get a free page for the process
    
    gfp_t gfp = GFP_KERNEL_ACCOUNT;
    void *virt_addr = (void*) get_zeroed_page(gfp);
    if (!virt_addr) {
        printk(KERN_ERR "Failed to allocate memory using vmalloc\n");
        return -ENOMEM;
    }

    // Get the physical address of the page
    unsigned long paddr = __pa(virt_addr);

out:
    return 0;
}

static void __exit sample_exit(void) {
    printk("[*] Goodbye world!.\n");
}

module_init(sample_init);
module_exit(sample_exit);
