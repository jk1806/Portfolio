/**
 * J-Link Implementation
 * Author: jk1806
 * Created: 2025-01-15
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init jlink_init(void)
{
    pr_info("jlink: Initializing\n");
    return 0;
}

static void __exit jlink_exit(void)
{
    pr_info("jlink: Exiting\n");
}

module_init(jlink_init);
module_exit(jlink_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("J-Link Implementation");
MODULE_VERSION(MODULE_VERSION);
