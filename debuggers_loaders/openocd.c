/**
 * OpenOCD Implementation
 * Author: jk1806
 * Created: 2025-01-05
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init openocd_init(void)
{
    pr_info("openocd: Initializing\n");
    return 0;
}

static void __exit openocd_exit(void)
{
    pr_info("openocd: Exiting\n");
}

module_init(openocd_init);
module_exit(openocd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("OpenOCD Implementation");
MODULE_VERSION(MODULE_VERSION);
