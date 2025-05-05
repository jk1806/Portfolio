/**
 * Hardware Abstraction Layer
 * Author: jk1806
 * Created: 2025-05-05
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init hal_init(void)
{
    pr_info("hal: Initializing\n");
    return 0;
}

static void __exit hal_exit(void)
{
    pr_info("hal: Exiting\n");
}

module_init(hal_init);
module_exit(hal_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("Hardware Abstraction Layer");
MODULE_VERSION(MODULE_VERSION);
