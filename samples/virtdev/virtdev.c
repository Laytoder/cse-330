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
MODULE_DESCRIPTION("Virtual Device Creation Example");
MODULE_VERSION("0.01");

/* Structures used to create a virtual device */
static dev_t                dev = 0;
static struct class*        virtdev_class;
static struct cdev          virtdev_cdev;

struct ioctl_data {
	int number;
};
#define EXAMPLE_IOCTL		_IOW('a', 'a', struct ioclt_data *)

/* IOCTL handler for vmod. */
static long sample_ioctl(struct file *f, unsigned int cmd, unsigned long arg) {
    struct ioctl_data kernel_buffer; 

    switch (cmd)
    {
    case EXAMPLE_IOCTL:    	
    	/* Copy data from user */
    	if(copy_from_user((void*) &kernel_buffer, (void*)arg, sizeof(struct ioctl_data))){
    	    printk("Error: User didn't send right message.\n");
            return -1;
    	}

        /* Debug message */
        printk("IOCTL: EXAMPLE_IOCTL(%x)\n", kernel_buffer.number);
    	break;
    default:
        printk("Error: incorrect IOCTL command.\n");
        return -1;
    }
    return 0;
}

/* Required definitions of file operations for IOCTL */
static struct file_operations fops = 
{
    .owner          = THIS_MODULE,
    .unlocked_ioctl = sample_ioctl,
};

/* Init and Exit functions */
static int __init sample_init(void) {
    /* Allocate a character device. */
    if (alloc_chrdev_region(&dev, 0, 1, "virtdev") < 0) {
        printk("error: couldn't allocate chardev region.\n");
        return -1;
    }
    printk("[*] Allocated chardev.\n");

    /* Initialize the chardev with my fops. */
    cdev_init(&virtdev_cdev, &fops);

    if (cdev_add(&virtdev_cdev, dev, 1) < 0) {
        printk("[x] Couldn't add virtdev cdev.\n");
        goto cdevfailed;
    }
    printk("[*] Allocated cdev.\n");

    if ((virtdev_class = class_create("virtdev_class")) == NULL) {
        printk("[X] couldn't create class.\n");
        goto cdevfailed;
    }
    printk("[*] Allocated class.\n");

    if ((device_create(virtdev_class, NULL, dev, NULL, "virtdev")) == NULL) {
        printk("[X] couldn't create device.\n");
        goto classfailed;
    }
    printk("[*] Virtual device added.\n");

    return 0;

classfailed:
    class_destroy(virtdev_class);
cdevfailed:
    unregister_chrdev_region(dev, 1);

    return -1;
}

static void __exit sample_exit(void) {
    /* Destroy the classes too (IOCTL-specific). */
    if (virtdev_class) {
        device_destroy(virtdev_class, dev);
        class_destroy(virtdev_class);
    }
    cdev_del(&virtdev_cdev);
    unregister_chrdev_region(dev,1);
    printk("[*] Virtual device removed.\n");
}

module_init(sample_init);
module_exit(sample_exit);