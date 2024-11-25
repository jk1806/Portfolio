/**
 * DoIP Implementation
 * Author: jk1806
 * Created: 2024-11-01
 * 
 * Implementation for embedded systems
 */

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_VERSION "1.0.0"

static int __init doip_init(void)
{
    pr_info("doip: Initializing\n");
    return 0;
}

static void __exit doip_exit(void)
{
    pr_info("doip: Exiting\n");
}

module_init(doip_init);
module_exit(doip_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("DoIP Implementation");
MODULE_VERSION(MODULE_VERSION);
