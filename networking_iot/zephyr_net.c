/**
 * Zephyr networking stack
 * Author: jk1806
 * Created: 2024-08-25
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init zephyr_net_init(void)
{
    pr_info("zephyr_net: Initializing\n");
    return 0;
}

static void __exit zephyr_net_exit(void)
{
    pr_info("zephyr_net: Exiting\n");
}

module_init(zephyr_net_init);
module_exit(zephyr_net_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("Zephyr networking stack");
MODULE_VERSION(MODULE_VERSION);
