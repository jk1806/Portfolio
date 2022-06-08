/**
 * Basic Device Driver Implementation
 * Author: jk1806
 * Created: 2022
 * 
 * Simple device driver for learning and development
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init basic_driver_init(void) {
    printk(KERN_INFO "Basic device driver loaded\n");
    return 0;
}

static void __exit basic_driver_exit(void) {
    printk(KERN_INFO "Basic device driver unloaded\n");
}

module_init(basic_driver_init);
module_exit(basic_driver_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Basic Device Driver");
MODULE_AUTHOR("jk1806");
